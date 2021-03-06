/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Giovanni De Toni, Heiko Strathmann, Thoralf Klein, 
 *          Soeren Sonnenburg, Sergey Lisitsyn, Roman Votyakov, Kyle McQuisten
 */

#include <shogun/base/progress.h>
#include <shogun/evaluation/CrossValidation.h>
#include <shogun/machine/Machine.h>
#include <shogun/mathematics/Statistics.h>
#include <shogun/modelselection/ModelSelectionParameters.h>
#include <shogun/modelselection/ParameterCombination.h>
#include <shogun/modelselection/RandomSearchModelSelection.h>

using namespace shogun;

CRandomSearchModelSelection::CRandomSearchModelSelection() : CModelSelection()
{
	set_ratio(0.5);
}

CRandomSearchModelSelection::CRandomSearchModelSelection(
		CMachineEvaluation* machine_eval,
		CModelSelectionParameters* model_parameters, float64_t ratio)
		: CModelSelection(machine_eval, model_parameters)
{
	set_ratio(ratio);
}

CRandomSearchModelSelection::~CRandomSearchModelSelection()
{
}

CParameterCombination* CRandomSearchModelSelection::select_model(bool print_state)
{
	if (print_state)
		SG_PRINT("Generating parameter combinations\n")

	/* Retrieve all possible parameter combinations */
	CDynamicObjectArray* all_combinations=
			(CDynamicObjectArray*)m_model_parameters->get_combinations();

	int32_t n_all_combinations=all_combinations->get_num_elements();
	SGVector<index_t> combinations_indices=CStatistics::sample_indices(n_all_combinations*m_ratio, n_all_combinations);

	CDynamicObjectArray* combinations=new CDynamicObjectArray();

	for (int32_t i=0; i<combinations_indices.vlen; i++)
		combinations->append_element(all_combinations->get_element(i));

	CCrossValidationResult* best_result=new CCrossValidationResult();

	CParameterCombination* best_combination=NULL;
	if (m_machine_eval->get_evaluation_direction()==ED_MAXIMIZE)
	{
		if (print_state) SG_PRINT("Direction is maximize\n")
		best_result->set_mean(CMath::ALMOST_NEG_INFTY);
	}
	else
	{
		if (print_state) SG_PRINT("Direction is minimize\n")
		best_result->set_mean(CMath::ALMOST_INFTY);
	}

	/* underlying learning machine */
	CMachine* machine=m_machine_eval->get_machine();

	/* apply all combinations and search for best one */
	for (auto i : SG_PROGRESS(range(combinations->get_num_elements())))
	{
		CParameterCombination* current_combination=(CParameterCombination*)
				combinations->get_element(i);

		/* eventually print */
		if (print_state)
		{
			SG_PRINT("trying combination:\n")
			current_combination->print_tree();
		}

		current_combination->apply_to_modsel_parameter(
				machine->m_model_selection_parameters);

		/* note that this may implicitly lock and unlockthe machine */
		CCrossValidationResult* result =
				(CCrossValidationResult*)(m_machine_eval->evaluate());

		if (result->get_result_type() != CROSSVALIDATION_RESULT)
			SG_ERROR("Evaluation result is not of type CCrossValidationResult!")

		if (print_state)
			result->print_result();

		/* check if current result is better, delete old combinations */
		if (m_machine_eval->get_evaluation_direction()==ED_MAXIMIZE)
		{
			if (result->get_mean() > best_result->get_mean())
			{
				if (best_combination)
					SG_UNREF(best_combination);

				best_combination=(CParameterCombination*)
						combinations->get_element(i);

				SG_REF(result);
				SG_UNREF(best_result);
				best_result=result;
			}
			else
			{
				CParameterCombination* combination=(CParameterCombination*)
						combinations->get_element(i);
				SG_UNREF(combination);
			}
		}
		else
		{
			if (result->get_mean() < best_result->get_mean())
			{
				if (best_combination)
					SG_UNREF(best_combination);

				best_combination=(CParameterCombination*)
						combinations->get_element(i);

				SG_REF(result);
				SG_UNREF(best_result);
				best_result=result;
			}
			else
			{
				CParameterCombination* combination=(CParameterCombination*)
						combinations->get_element(i);
				SG_UNREF(combination);
			}
		}

		SG_UNREF(result);
		SG_UNREF(current_combination);
	}

	SG_UNREF(best_result);
	SG_UNREF(machine);
	SG_UNREF(combinations);

	return best_combination;
}
