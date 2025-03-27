#ifndef BOSCH_BMI270_INTERRUPT_H
#define BOSCH_BMI270_INTERRUPT_H

/** @brief          
 *  @param p_dev    
 *  @return         0 when successful
*/
int bmi2_init_interrupt(const struct device *p_dev);

/** @brief          
 *  @param p_dev    
 *  @param p_trig   
 *  @param handler  
 *  @return         0 when successful
*/
int bmi270_trigger_set(const struct device *p_dev, const struct sensor_trigger *p_trig, sensor_trigger_handler_t handler);

#endif /* BOSCH_BMI270_INTERRUPT_H */
