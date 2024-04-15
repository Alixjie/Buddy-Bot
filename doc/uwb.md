# UWB

In this project we use a UWB sensor, it could acquire the distance and degree between the anchor and the tag every 96ms.

## ULM3PDOAComm

This is the driver class of ULM3 PDOA sensor.

This class implement following life cycle:

1. In constructor function, it try to open the corresponding file (/dev/ttyUSB\*) of serial port through pre-setted parameters (using blocking I/O method).
2. In constructor function, it also sets the serial settings, such as baud rate and reading mode (the communication protocol is 115200bps-8-n-1). In this project using canonical method since the data from sensor is end by **"\r\n"**.
3. Create a new thread that continuously fetches data from the serial port to ensure that the Raspberry Pi can perform other functions. And store the handle of that thread.
4. Every time acquisite a data, call the _hasSample()_ function in callback class.
5. Call the _stop()_ function in old thread can stop the data-acquire process.
6. In the deconstructor function, it will call the _stop()_ to stop data fetch, make data fetch thread join to the old thread to completely end the process, and close the device file.

In conclusion, this class is in charge of creating a new thread to continuously acquire data from serial port.

The function of this class:

- ULM3PDOAComm(const char \*port_name);
- ~ULM3PDOAComm();
- void registerCallback(ULM3PDOACallback \*cb);
- void unregisterCallback();
- void start();
- void stop();

## ULM3AcquisitionCallback

This function is response to process the char array transmitted from ULM3PDOAComm.

These char array looks like this:
**MPxxxx,tag_id,x_cm,y_cm,distance_cm,RangeNumber,pdoa_deg,aoa_deg,distance_offset_cm,pdoa_offset_deg\r\n**

All we needed is _distance_cm, aoa_deg_, the distance and degree between anchor and tag.

The input is char[], thus we can add a '\0' to the end and use _sscanf_ to get data from input array.

The life cycle of this class:

1. In constructor, set _split_buffer_ = **nullptr**. Set a flag _isFirst_ ture. That means is the first time acquire char array.
2. Get the char array and use a function to pretreament it to get split data.
3. If this is the first acquire data, new a memory which the length is same as buffer in ULM3PDOAComm to the _split_buffer_. This pointer used to store the current string from ULM3PDOAComm.
4. Make some simple adjustments to the data.
5. Push the data to a synchronous queue.
6. If the push operation takes longer than the preset value, here it is 500ms, discard this data and the data in the queue.
7. In deconstructor, if _split_buffer_ unequal to nullptr then delete that memory to prevent memory leaks.

**Note 1**: The step 4 adjustment the data. In the previous versions, we think this class need to give the speed to motor, only two kinds of speed: stop or running, while in later we change the design, but in that time we have a lot of code, thus I did not change the struct, I just didn't use the speed property.

**Note 2**: In the step 6. Push the data to a synchronous queue. This is a circular queue with a fixed length so when queue is full, the writer thread (this thread) must wait for the customer thread (other data which wants to get UWB data) to pop the front data. Therefore if push operation takes more than we need to judge, if pushing this data wait for more than a pre-setted time (500ms) cancel this push. This function will return a bool variable, false means timeout, and I can determine discard all data in queue or not.

The function of this class:

- ULM3AcquisitionCallback();
- ~ULM3AcquisitionCallback();
- virtual void hasSample();
- void getSample(const simple_string &sip_str);

## SyncQueue

This is a circular synchronized queue work for multi-threaded interactions.

This queue use mutex and conditional variable to implement mutual exclusion and wake up blocked thread.

This is a template class so it should write in a head file instead split to source file and head file.

This class has following functions:

- SyncQueue(const std::size_t capacity);
- void push(const value_type &item);
- void push(const value_type &item, uint delay);
- value_type waitAndPop();
- void waitAndPop(uint l, value_type \*items);
- bool tryPop(value_type &item);
- value_type frontItem();
- std::size_t size();
- void clear();
- std::size_t readSize();

There is a vector<value_type> to store data.
In the constructor function, I use reserve to ask vector to allocate a memory with capacity + 1 since the circular needs one more empty site to distinguish queue is full or empty.

The **void push(const value_type &item)** function is used to push a data to queue. However this queue has the fixed length, so it should wait for queue not full. I use _std::condition_variable_ to implement this.

The **void push(const value_type &item, uint delay)** function is the overload of above function, and it can will only wait for delay ms, if time out, discard this data and return **false** otherwise push the data normally.

The **value_type waitAndPop()** will wait for queue not empty and pop an item.

The **void waitAndPop(uint l, value_type \*items)** will wait for queue untill there are not less than _l_ items and pop _l_ items.

The **bool tryPop(value_type &item)** will try to pop an item, if could not pop, return false immediately.

The **value_type frontItem()** will return the front item but do not pop it.

The **std::size_t size()** will return the current size by _((back + (capacity + 1)) - front) % (capacity + 1)_ if front >= back or _front-back_ if back < front.

The **void clear()** clear the queue by set front=back=0.

The **std::size_t readSize()** is a private function to get size without lock mutex while size() must lock mutex.

## ULM3Samples

This is the wrap class of above classes. It provide some interface to the outside. The most important is _getData()_ and _controlCar_. The first one is used to return the distance and degree between anchor and tag, and the second one is to indicate where should car go.

Both of these functions are get the data from sync queue. In other words this is a typical producer-consumer model.
