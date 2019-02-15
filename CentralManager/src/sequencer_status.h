#ifndef _SEQ_STATUS_H_
#define _SEQ_STATUS_H_

#include <string>

#include "../thirdparty/rapidjson/writer.h"         // rapidjson
#include "../thirdparty/rapidjson/stringbuffer.h"   // rapidjson

/* This a POD (Plain Old Data) struct used for to hold the sequencer status infomation.
 * It tehcically is not a POD, but the function only converts the internal data into a 
 * JSON string, so for the most part it is.
 */
struct sequencer_status {
        friend bool operator ==  (const sequencer_status &, const sequencer_status &);
        friend bool operator !=  (const sequencer_status &, const sequencer_status &);
        void make_JSON(std::string & output);
        void make_JSON_diff(std::string & output, const sequencer_status & input);

        std::string time;
        unsigned int current_state;
};

#endif