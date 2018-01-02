# RedisClient

Arduino client for Redis key-value store.

## Features
* Based on Arduino Ethernet Library
* One persistent TCP connection to the Redis server
* Supported different Redis commands (see sourcecode, more to come)
* Very low memory consumption as packets are constructed on Ethernet chip
* Simple parsing of results

## Interface
Some commands do not require any additional arguments, they are used like this:

    client.INCR("sequence");
    client.GET("key");

Sometimes the result is available direct (like for INCR) but sometimes you'll have to request the value using a specific function (see documentation in redisClient.h).

Other commands do require more data which has to be supplied using multiple calls to redisClient to keep the memory consumption low. These commands always begin with "start".

	client.startPUBLISH("channel");
	client.startRPUSH("list");
	
These commands do require more arguments. Bad things will happen if you forget to supply the correct number of arguments using the sendArg(…) methods.

	client.sendArg("a string");        // strings
	client.sendArg(buffer, buffer_len);// binary data
	client.sendArg(20);                // Integer values
	
You do have to call a specify function to finish these commands. They are all named end* and always return 1 on success, 0 on failure. Some of them pass additional informations to pointers supplied to the method.

    uint8_t endPUBLISH(uint16_t *subscribers);
    uint8_t endPUBLISH();
    uint8_t endHSET();
    uint8_t endRPUSH(uint16_t *listitems);
    uint8_t endRPUSH();

If above methods return 0 you may use these methods to get more details about the actual result which is most likely an error message

	client.resultType();               // Returns a value from RedisResult enumeration
	client.resultStatus(char *buffer);	// Reads status result (mostly OK) into buffer
	client.resultError(char *buffer);  // Reads error description into buffer
	client.resultBulk(char *buffer, uint16_t buffer_size); // reads back BULK reply
	
You may repeat client.resultType() as often as you like but all the other result* commands _consume_ the result and prepare the connection for the next command.


## Example Sketch
A quite ugly example but it shows how to use the results and has some basic error handling.

	IPAddress server(192, 168, 42, 10);
	RedisClient client(server);
	
	void setup() {
		Serial.begin(57600);
		Ethernet.begin(/* add Ethernet options here */);
		client.connect();
		
		// Query a value
		char buffer[100];
		client.GET("samplekey");
		client.resultBulk(buffer, 100);
		Serial.println(buffer);
	}
	
	void loop() {

		Serial.print("Publishing to pub/sub channel... ");
	    client.startPUBLISH("test");
	    client.sendArg("some data");
	    uint16_t subscribers;
    	if (client.endPUBLISH(&subscribers)) {
	    	Serial.print("Sent to ");
    		Serial.print(subscribers, DEC);
	    	Serial.println(" subscribers.");
    	} else
			Serial.println("FAILED!");

	    Serial.print("Getting new ID from Redis... ");
    	uint16_t newid = client.INCR("sequence");
	    Serial.println(newid);
    
	    Serial.print("Adding data to new hash... ");
    	client.startHSET("rawin", newid);
	    client.sendArg("data");
    	client.sendArgRFMData("some data here");
	    if (client.endHSET())
    		Serial.println("ok");
	    else
    		Serial.println("FAILURE!");
    
    	Serial.print("Adding new hash to fifo... ");
	    client.startRPUSH("testlist");
    	client.sendArg(newid);
    	uint16_t listitems;
	    if (client.endRPUSH(&listitems)) {
      		Serial.print("Now ");
      		Serial.print(listitems, DEC);
      		Serial.println(" items in list.");
    	} else
      		Serial.println("FAILURE!");

	    if (listitems > 100) {
    	  	Serial.print("Cutting list to 100 items... ");
	      	if (client.LTRIM("testlist", 0, 99))
	      		Serial.println("ok");
	      	else
	      		Serial.println("FAILURE!");
    	}
	}