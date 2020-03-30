/**
 * @file NicheBreadth.h
 * @brief Class NicheBreadth. A class of the niche breadth of a virtual species
 * @author Huijie Qiao
 * @version 1.0
 * @date 3/13/2020
 * @details
 * Copyright 2014-2020 Huijie Qiao
 * Distributed under GNU license
 * See file LICENSE for detail or copy at https://www.gnu.org/licenses/gpl-3.0.en.html
 *
 */


#ifndef DEFINITIONS_NICHEBREADTH_H_
#define DEFINITIONS_NICHEBREADTH_H_
/**
 * @brief A class of the niche breadth of a virtual species
 */
class NicheBreadth {
private:
    double min;
    double max;
public:
    /**
	 * @brief Constructor of NicheBreadth class
	 * @param min lower limit of the niche breadth
	 * @param max upper limit of the niche breadth
	 */
    NicheBreadth(double min, double max);
    /**
	 * @brief Destructor of NicheBreadth class
	 *
	 * release all the resources
	 */
    virtual ~NicheBreadth();
    /**
     * @brief return the upper limit of the niche breadth
     * @return
     */
    double getMax();
    /**
	 * @brief return the lower limit of the niche breadth
	 * @return
	 */
    double getMin();
    void setMax(double v);
    void setMin(double v);

};

#endif /* DEFINITIONS_NICHEBREADTH_H_ */
