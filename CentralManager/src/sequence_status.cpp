

// Uses RapidJSON to converts sequence_status struct to a vector. 
int sequence_status::
make_JSON(std::vector<char> & output) {
    using namespace rapidjson;

    Document new_json;

    // makes json object not a string
    new_json.SetObject();

    // make allocator
    Document::AllocatorType& allocator = new_json.GetAllocator();

    // time
    new_json.AddMember("time_ms", time, allocator);

    // sensor values, only change these by adding or removing members
    new_json.AddMember("current_state", current_state, allocator);

    // converts json document to string
    StringBuffer str_buff;
    Writer<StringBuffer> writer(str_buff);
    new_json.Accept(writer);

    std::string temp = str_buff.GetString();

    // resize vector if needed
    unsigned int len = temp.size();
    if(output.size() < len+2) {
        output.resize(len+2);
    }

    // copy into output vector
    for(unsigned int i=0; i<temp.size(); ++i)
        output[i] = temp[i];

    // add \n\0 chars to vector
    output[len] = '\n';
    output[len+1] = '\0';

    return 1;
}


/* Uses RapidJSON to converts sequence_status struct to a vector. 
 * Only adds data that has changed to reduce data sent to clients
 */
int sequence_status::
make_JSON_diff(const sequence_status & other, std::vector<char> & output) {
    using namespace rapidjson;

    Document new_json;

    // makes json object not a string
    new_json.SetObject();

    // make allocator
    Document::AllocatorType& allocator = new_json.GetAllocator();

    // time
    new_json.AddMember("time_ms", time, allocator);

    // sensor values, only change these by adding or removing members
    if(current_state != other.current_state)
        new_json.AddMember("current_state", current_state, allocator);

    // converts json document to string
    StringBuffer str_buff;
    Writer<StringBuffer> writer(str_buff);
    new_json.Accept(writer);

    std::string temp = str_buff.GetString();

    // resize vector if needed
    unsigned int len = temp.size();
    if(output.size() < len+2) {
        output.resize(len+2);
    }

    // copy into output vector
    for(unsigned int i=0; i<temp.size(); ++i)
        output[i] = temp[i];

    // add \n\0 chars to vector
    output[len] = '\n';
    output[len+1] = '\0';

    return 1;
}
