/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Soeren Sonnenburg, Sergey Lisitsyn, Heiko Strathmann,
 *          Saurabh Mahindre, Fernando Iglesias, Evgeniy Andreev, Yuyu Zhang,
 *          Chiyuan Zhang, Thoralf Klein
 */

#ifndef _BINARY_LABELS__H__
#define _BINARY_LABELS__H__

#include <shogun/lib/config.h>

#include <shogun/labels/DenseLabels.h>
#include <shogun/labels/LabelTypes.h>
#include <shogun/labels/MulticlassLabels.h>
#include <shogun/lib/common.h>

namespace shogun
{
class CFile;
template <class T> class SGVector;

/** @brief Binary Labels for binary classification
 *
 * valid values for labels are +1/-1
 *
 * Scores may be converted into calibrated probabilities using
 * scores_to_probabilities(), which implements the method described in
 * Lin, H., Lin, C., and Weng, R. (2007).
 * A note on Platt's probabilistic outputs for support vector machines.
 * Should only be used in conjunction with SVM.
 */
class CBinaryLabels : public CDenseLabels
{
public:
	/** default constructor */
	CBinaryLabels();

	/** constructor
	 *
	 * @param num_labels number of labels
	 */
	CBinaryLabels(int32_t num_labels);

#if !defined(SWIGJAVA) && !defined(SWIGCSHARP)
	/** constructor
	 * sets labels with src elements
	 *
	 * @param src labels to set
	 */
	CBinaryLabels(SGVector<int32_t> src);

	/** constructor
	 * sets labels with src elements (int64 version)
	 *
	 * @param src labels to set
	 */
	CBinaryLabels(SGVector<int64_t> src);
#endif

	/** constructor
	 * sets values from src vector
	 * sets labels with sign of src elements with added threshold
	 *
	 * @param src labels to set
	 * @param threshold threshold
	 */
	CBinaryLabels(SGVector<float64_t> src, float64_t threshold = 0.0);

	/** constructor
	 *
	 * @param loader File object via which to load data
	 */
	CBinaryLabels(CFile * loader);

	/** Cast constructor
	 *
	 * @param dense CDenseLabels containing the binary labels
	 */
	CBinaryLabels(const CDenseLabels& dense);

	/** Make sure the label is valid, otherwise raise SG_ERROR.
	 *
	 * possible with subset
	*
	* @param context optional message to convey the context
	 */
	virtual void ensure_valid(const char * context = NULL);

	/** get label type
	 *
	 * @return label type binary
	 */
	virtual ELabelType get_label_type() const;

	/** Converts all scores to calibrated probabilities by fitting a
	 * sigmoid function using the method described in
	 * Lin, H., Lin, C., and Weng, R. (2007).
	 * A note on Platt's probabilistic outputs for support vector machines.
	 *
	 * A sigmoid is fitted to the scores of the labels and then is used
	 * to compute porbabilities which are stored in the values vector. This
	 * is done via computing
	 * \f$pf=x*a+b\f$ for a given score \f$x\f$ and then computing
	 * \f$\frac{\exp(-f)}{1+}exp(-f)}\f$ if \f$f\geq 0\f$ and
	 * \f$\frac{1}{(1+\exp(f)}\f$ otherwise, where \f$a, bf\f$ are shape parameters
	 * of the sigmoid. These can be specified or learned automatically
	 *
	 * Should only be used in conjunction with SVM.
	 *
	 * @param a parameter a of sigmoid, if a=b=0, both are learned
	 * @param b parameter b of sigmoid, if a=b=0, both are learned
	 */
	void scores_to_probabilities(float64_t a = 0, float64_t b = 0);

	/** @return object name */
	virtual const char * get_name() const
	{
		return "BinaryLabels";
	}

#ifndef SWIG // SWIG should skip this part
	virtual CLabels* shallow_subset_copy();
#endif
};

#ifndef SWIG
Some<CBinaryLabels> binary_labels(CLabels* orig);
#endif // SWIG
}
#endif
