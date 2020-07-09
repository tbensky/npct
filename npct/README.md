# ESP32 Software for No Phone Contact Tracer

The software for the ESP works to implement "the plan" bullet points above. It accomplishes two primary functions:

* Starts a Bluetooth low-energy (BLE) server. This allows for outside connection to the ESP32 device by the user for configuration and reading of the device. "Configuration" here means to be able to set the BLE name of the ESP32 with a unique ID and health-status code (of the user). This BLE name (hence health information) is continually advertised by the ESP32. "Reading" here means to retrieve health related contacts that may have been received. 

* Starts a BLE discovery scan, to continually scan for the names of other nearby BLE devices. All contact tracing names as used by this system, start with the hash-tag "#C19:".  Any names not starting with this are ignored. Those starting with it are logged into the ESP32's internal RAM. About 5,000 such names can be logged.
