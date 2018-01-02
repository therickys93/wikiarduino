#include "redisClient.h"


RedisClient::RedisClient(IPAddress srv) {
    _srv = srv;
}

uint8_t RedisClient::connect() {
    if (connected()) {
        flushResult();
        return 1;
    }
    
    return EthernetClient::connect(_srv, REDIS_PORT);
}


uint16_t RedisClient::INCR(char* key) {
    connect();
    
    _resType = RedisResult_NOTRECEIVED;
    startCmd(2);
    sendArg("INCR");
    sendArg(key);
    
    return resultInt();
}


uint8_t RedisClient::LTRIM(char* list, int16_t start, int16_t stop) {
    connect();
    
    _resType = RedisResult_NOTRECEIVED;
    startCmd(4);
    sendArg("LTRIM");
    sendArg(list);
    sendArg(start);
    sendArg(stop);
    
    return resultType() == RedisResult_SINGLELINE;
}


uint8_t RedisClient::GET(char* key) {
    connect();
    
    _resType = RedisResult_NOTRECEIVED;
    startCmd(2);
    sendArg("GET");
    sendArg(key);
    
    return resultType() == RedisResult_BULK;
}


uint8_t RedisClient::startPUBLISH(char* channel) {
    connect();
    
    _resType = RedisResult_NOTRECEIVED;
    startCmd(3);
    sendArg("PUBLISH");
    sendArg(channel);
}


uint8_t  RedisClient::startHSET(char* key) {
    connect();
    
    _resType = RedisResult_NOTRECEIVED;
    startCmd(4);
    sendArg("HSET");
    sendArg(key);
}

uint8_t RedisClient::endPUBLISH(uint16_t *subscribers) {
    if (resultType() == RedisResult_INTEGER) {
        *subscribers = resultInt();
        return 1;
    } else {
        return 0;
    }
}

uint8_t RedisClient::endPUBLISH() {
    uint16_t subscribers;
    return endPUBLISH(&subscribers);
}


uint8_t RedisClient::endHSET() {
    if (resultType() == RedisResult_INTEGER) {
        resultInt(); // throw away result
        return 1;
    } else {
        return 0;
    }
}


uint8_t RedisClient::endRPUSH(uint16_t *listitems) {
    if (resultType() == RedisResult_INTEGER) {
        *listitems = resultInt();
        return 1;
    } else {
        return 0;
    }
}

uint8_t RedisClient::endRPUSH() {
    uint16_t listitems;
    return endPUBLISH(&listitems);
}




uint8_t  RedisClient::startHSET(char* key, uint16_t index) {
    char buffer[30];
    uint8_t offset = addString(buffer, 0, key);
    offset = addChar(buffer, offset, ':');
    offset = addInt(buffer, offset, index);
    offset = addChar(buffer, offset, '\0');
    startHSET(buffer);
}


uint8_t RedisClient::startRPUSH(char* list) {
    connect();
    
    _resType = RedisResult_NOTRECEIVED;
    startCmd(3);
    sendArg("RPUSH");
    sendArg(list);
}




RedisResult RedisClient::resultType() {
    if (_resType == RedisResult_NOTRECEIVED) {
        while(! available() )
            delay(1);
        
        switch( read() ) {
            case  '+': _resType = RedisResult_SINGLELINE; break;
            case  '-': _resType = RedisResult_ERROR; break;
            case  ':': _resType = RedisResult_INTEGER; break;
            case  '$': _resType = RedisResult_BULK; break;
            case  '*': _resType = RedisResult_MULTIBULK; break;
        }
    }
    
    if (_resType > RedisResult_NOTRECEIVED)
        return _resType;
    
    return RedisResult_NONE;
}


uint16_t RedisClient::readInt() {
    uint16_t res = 0;
    char chr;
    
    while(1) {
        while(! available() )
            delay(1);
        
        chr = read();
        if (chr >= '0' && chr <= '9') {
            res *= 10;
            res += chr - '0';
        } else if (chr == '\r') {
            // ignore
        } else if (chr == '\n') {
            _resType = RedisResult_NONE;
            return res; // returning result
        }
    }
}


uint16_t RedisClient::resultInt() {
    if (resultType() != RedisResult_INTEGER)
        return 0;
    
    return readInt();
}


uint16_t RedisClient::readSingleline(char *buffer) {
    char chr;
    uint8_t offset = 0;
    
    while(1) {
        while(! available() )
            delay(1);
        
        chr = read();
        if (chr >= 32 && chr <= 126) {
            buffer[offset++] = chr;
        } else if (chr == '\r') {
            // ignore
        } else if (chr == '\n') {
            _resType = RedisResult_NONE;
            buffer[offset++] = '\0';
            return offset; // returning length of string
        }
    }
}


uint16_t RedisClient::resultStatus(char *buffer) {
    if (resultType() != RedisResult_SINGLELINE)
        return 0;
    
    return readSingleline(buffer);
}


uint16_t RedisClient::resultError(char *buffer) {
    if (resultType() != RedisResult_ERROR)
        return 0;
    
    return readSingleline(buffer);
}


// Note: buffer_size has to be bigger than result as result there'll be always a \0 appendet
uint16_t RedisClient::resultBulk(char *buffer, uint16_t buffer_size) {
    if (resultType() != RedisResult_BULK)
        return 0;
    
    uint16_t result_size = readInt();
    while(available() < result_size+2)
        delay(1);
    
    read((uint8_t*)buffer, result_size);
    buffer[result_size] = '\0';
    
    // throw away newline
    read(); read();
    
    return result_size;
}


void RedisClient::flushResult() {
    switch (resultType()) {
        case RedisResult_SINGLELINE:
        case RedisResult_ERROR:
            char buffer[200];
            readSingleline(buffer);
            break;
            
        case RedisResult_INTEGER:
            readInt();
            break;
    }
}



inline uint8_t RedisClient::addChar(char* buffer, uint8_t offset, char chr) {
    buffer[offset] = chr;
    return offset+1;
}

inline uint8_t RedisClient::addData(char *buffer, uint8_t offset, char* str, uint8_t len) {
    memcpy( buffer+offset, str, len);
    return offset + len;
}

inline uint8_t RedisClient::addString(char *buffer, uint8_t offset, char* str) {
    return addData(buffer, offset, str, strlen(str));
}

inline uint8_t RedisClient::addInt(char *buffer, uint8_t offset, int num) {
    itoa(num, buffer+offset, 10);
    return offset + strlen(buffer+offset);
}

inline uint8_t RedisClient::addNewline(char *buffer, uint8_t offset) {
    buffer[offset++] = '\r';
    buffer[offset++] = '\n';
    return offset;
}

void RedisClient::sendArg(uint8_t* arg, uint8_t len) {
    char buffer[20];
    uint8_t offset = 0;
    offset = addChar(buffer, offset, '$');
    offset = addInt(buffer, offset, len);
    offset = addNewline(buffer, offset);
    write((uint8_t*)buffer, offset);
    
    write(arg, len);
    
    offset = 0;
    offset = addNewline(buffer, offset);
    write((uint8_t*)buffer, offset);
}

void RedisClient::sendHashArg(char* key, uint16_t index) {
    char buffer[30];
    uint8_t offset = addString(buffer, 0, key);
    offset = addChar(buffer, offset, ':');
    offset = addInt(buffer, offset, index);
    sendArg((uint8_t*)buffer, offset);
}

void RedisClient::sendArg(char* arg) {
    sendArg((uint8_t*)arg, strlen(arg));
}

void RedisClient::sendArg(int arg) {
    char buffer[20];
    uint8_t offset = addInt(buffer, 0, arg);
    sendArg((uint8_t*)buffer, offset);
}

void RedisClient::startCmd(uint8_t num_args) {
    char buffer[20];
    uint8_t offset = 0;
    
    // preparing argument count
    offset = addChar(buffer, offset, '*');
    offset = addInt(buffer, offset, num_args);
    offset = addNewline(buffer, offset);
    write((uint8_t*)buffer, offset);  
}

void RedisClient::sendArgRFMData(uint8_t header, uint8_t *data, uint8_t data_len) {
    char buffer[200];
    uint8_t offset = 0;
    
    offset = addString(buffer, offset, "OK ");
    offset = addInt(buffer, offset, header);
    for (int i=0; i<data_len; i++) {
        offset = addChar(buffer, offset, ' ');
        offset = addInt(buffer, offset, data[i]);
    }
    
    sendArg((uint8_t*)buffer, offset);
}