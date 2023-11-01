#include "utils.hpp"

DetailedTime minutesToDetailedTime(unsigned int minutes){
    DetailedTime detailedTime;
    detailedTime.years = minutes / 60 / 24 / 365;
    detailedTime.days = minutes / 60 / 24 % 365;
    detailedTime.hours = minutes / 60 % 24;
    detailedTime.minutes = minutes % 60;
    return detailedTime;
}