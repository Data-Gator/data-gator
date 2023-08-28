#include "TimeStamp.hpp"

/*
 * Construct a TimeStamp from epoch time(seconds since 1970)
 */
TimeStamp::TimeStamp(time_t epoch_time){
    // store
    this->epoch_time = epoch_time;

    // convert to mm/dd/yy...etc
    this->ptm = *gmtime ((time_t *)&epoch_time);

    this->day = this->ptm.tm_mday;
    this->month = this->ptm.tm_mon+1;
    this->year = this->ptm.tm_year+1900;

    // get hours, minutes, seconds
    this->hour = this->ptm.tm_hour;
    this->minutes = this->ptm.tm_min;
    this->seconds = this->ptm.tm_sec;

    this->offset = 0;
}

/*
 * Construct TimeStamp by converting mm/dd/yyThh:mm:ss+offset to timestamp
 */
TimeStamp::TimeStamp(string ts){
    int t = ts.find("T");
    int cross = ts.find("+");

    int first_d = ts.find("-");
    this->month = stoi(ts.substr(0, first_d));
    int second_d = ts.find("-", first_d + 1);
    this->day = stoi(ts.substr(first_d + 1, second_d - first_d));
    this->year = stoi(ts.substr(second_d + 1, t - second_d));

    int first_c = ts.find(":");
    this->hour = stoi(ts.substr(t+1, first_c - t));
    int second_c = ts.find(":", first_c + 1);
    this->minutes = stoi(ts.substr(first_c+1, second_c - first_c));

    if(cross == string::npos){
        // if offset is not present, take seconds to end of string
        this->seconds = stoi(ts.substr(second_c+1));

    }else{
        this->seconds = stoi(ts.substr(second_c+1, cross - second_c));
    }

    this->offset = stoi(ts.substr(cross+1));

    ptm = tm{0};
    // convert to tm struct
    this->ptm.tm_year = this->year-1900;  // This is year-1900, so 112 = 2012
    this->ptm.tm_mon = this->month-1;
    this->ptm.tm_mday = this->day;
    this->ptm.tm_hour = this->hour;
    this->ptm.tm_min = this->minutes;
    this->ptm.tm_sec = this->seconds;

    // convert to epoch time
    this->epoch_time = std::mktime(&ptm);

}

/*
 * Construct TimeStamp from integer field values
 */
TimeStamp::TimeStamp(int day, int month, int year, int hour, int minutes, int seconds, int offset){
    ptm = tm{0};
    // convert to time since epoch
    this->ptm.tm_year = year-1900;  // This is year-1900, so 112 = 2012
    this->ptm.tm_mon = month-1;
    this->ptm.tm_mday = day;
    this->ptm.tm_hour = hour;
    this->ptm.tm_min = minutes;
    this->ptm.tm_sec = seconds;
    this->epoch_time = std::mktime(&ptm);

    // store
    this->day = day;
    this->month = month;
    this->year = year;
    this->hour = hour;
    this->minutes = minutes;
    this->seconds = seconds;
}

/*
 * Get string in form "mm/dd/yyThh:mm:ss+offset"
 */
string TimeStamp::to_string(){
    return std::to_string(this->month) + "-" + std::to_string(this->day) + "-" + std::to_string(this->year) + "T" + 
        std::to_string(this->hour) + ":" + std::to_string(this->minutes) + ":" + std::to_string(this->seconds) + "+" + 
        std::to_string(this->offset);
} 

/*
 * Get string with month, day, year in "mm-dd-yy" format
 */
string TimeStamp::get_mdy(){
    string mdy = std::to_string(this->month) + "-" + 
        std::to_string(this->day) + "-" + 
        std::to_string(this->year);

    return mdy;
}

TimeStampBuilder::TimeStampBuilder(NTPClient* ntp_client){
    this->ntp_client = ntp_client;
}

std::string TimeStampBuilder::get_date_time(){

    //if(this->ntp_client == NULL){return "00-00-00T00:00:00+0";}
    //return "00-00-00T00:00:00+0";

    //this->ntp_client.update();
    try{
        time_t epochTime = this->ntp_client->getEpochTime();
        TimeStamp ts(epochTime);
        return ts.to_string();

    }catch(...){
        Serial.println("ERROR WITH NTP UPDATE!!!");
    }

    return "00-00-00T00:00:00+0";

}

std::string TimeStampBuilder::get_date_time(std::string date, std::string time, std::string offset){
    return date + "T" + time + "+" + offset;
}

std::string TimeStampBuilder::format_time(int hours, int minutes, int seconds){
    return std::to_string(hours) + ":" + 
        std::to_string(minutes) + ":" + 
        std::to_string(seconds);
}

std::string TimeStampBuilder::format_date(unsigned long epoch_time){

    time_t epochTime = epoch_time;

    // get day, month, year
    struct tm* ptm = gmtime ((time_t *)&epochTime);
    int month_day = ptm->tm_mday;
    int current_month = ptm->tm_mon+1;
    int current_year = ptm->tm_year+1900;

    // get hours, minutes, seconds
    int hours = ptm->tm_hour;
    int minutes = ptm->tm_min;
    int seconds = ptm->tm_sec;

    int offset = 0;

    return std::to_string(current_month) + "-" + 
        std::to_string(month_day) + "-" + 
        std::to_string(current_year);
}


TimeStampParser::TimeStampParser(std::string timestamp){
    this->timestamp = timestamp;
}

int TimeStampParser::get_day(){
    int start = this->timestamp.find_first_of('-') + 1;
    int end = this->timestamp.find('-', start+1);

    std::string substr = this->timestamp.substr(start, end - start);

    return std::stoi(substr);
}
int TimeStampParser::get_month(){
    int len = this->timestamp.find('-');

    return std::stoi(this->timestamp.substr(0, len));
}

int TimeStampParser::get_year(){
    int start = this->timestamp.find_last_of('-') + 1;
    int len = this->timestamp.find('T') - start;

    return std::stoi(this->timestamp.substr(start, len));
}

int TimeStampParser::get_hours(){
    int start = this->timestamp.find('T') + 1;
    int len = this->timestamp.find_first_of(':') - start;

    return std::stoi(this->timestamp.substr(start, len));
}

int TimeStampParser::get_minutes(){
    int start = this->timestamp.find(':') + 1;
    int len = this->timestamp.find_last_of(':') - start;
    
    return std::stoi(this->timestamp.substr(start, len));
}

int TimeStampParser::get_seconds(){
    int start = this->timestamp.find_last_of(':') + 1;
    int len = this->timestamp.find('+') - start;

    return std::stoi(this->timestamp.substr(start, len));
}

int TimeStampParser::get_offset(){
    int start = this->timestamp.find('+') + 1;

    return std::stoi(this->timestamp.substr(start, std::string::npos));
}

