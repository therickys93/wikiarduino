#ifndef REDISCLIENT_H
#define REDISCLIENT_H

#include "EthernetClient.h"


#define REDIS_PORT 6379

enum RedisResult {
    RedisResult_NONE,
    RedisResult_NOTRECEIVED,
    RedisResult_SINGLELINE,
    RedisResult_ERROR,
    RedisResult_INTEGER,
    RedisResult_BULK,
    RedisResult_MULTIBULK
};


class RedisClient : public EthernetClient {
    
public:
    RedisClient(IPAddress srv);
    uint8_t connect();
    
    
    // simple commands not needing additional arguments
    uint16_t INCR(char* key);  // returns incremented number
    uint8_t  LTRIM(char* list, int16_t start, int16_t stop); // returns 1 on success
    uint8_t  GET(char* key);   // returns 1 on success, get value using resultBulk(buffer, buflen)
    
    
    // commands needing arguments using sendArg(...) and ending using end*
    uint8_t startPUBLISH(char* channel); // needs one argument
    uint8_t startHSET(char* key);        // needs TWO arguments
    uint8_t startHSET(char* key, uint16_t index); // needs TWO arguments
    uint8_t startRPUSH(char* list);      // needs one argument
    
    
    // ... to send arguments for commands above
    void sendArg(char* arg); // to send string which is \0 terminated
    void sendArg(uint8_t* arg, uint8_t len);
    void sendArg(int arg);
    void sendArgRFMData(uint8_t header, uint8_t *data, uint8_t data_len); // format RFM12B packet
    
    
    // end commands started using start*
    // all return 1 on success, 0 on failure
    uint8_t endPUBLISH(uint16_t *subscribers);
    uint8_t endPUBLISH();
    uint8_t endHSET();
    uint8_t endRPUSH(uint16_t *listitems);
    uint8_t endRPUSH();
    
    
    // read back results
    RedisResult resultType();
    
    uint16_t resultInt();
    uint16_t resultStatus(char *buffer);
    uint16_t resultError(char *buffer);
    uint16_t resultBulk(char *buffer, uint16_t buffer_size);
    
    
private:
    IPAddress _srv;
    uint8_t _results_waiting;
    RedisResult _resType;
    
    // internal methods for string manipulation
    inline uint8_t addChar(char* buffer, uint8_t offset, char chr);
    inline uint8_t addData(char *buffer, uint8_t offset, char* str, uint8_t len);
    inline uint8_t addString(char *buffer, uint8_t offset, char* str);
    inline uint8_t addInt(char *buffer, uint8_t offset, int num);
    inline uint8_t addNewline(char *buffer, uint8_t offset);
    
    // internal methods for construction redis packets in Ethernet Chip's memory
    void startCmd(uint8_t num_args);
    void sendHashArg(char* key, uint16_t index);
    
    // internal commands to parse redis results
    uint16_t readSingleline(char *buffer);
    uint16_t readInt();
    void flushResult();
};

#endif