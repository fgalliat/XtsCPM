rem javac -d bin -cp src;libs/jssc.jar src\TerminalFrame.java
del /s /q bin\*
javac -d bin -cp src;libs/jssc.jar src\SimpleSender.java
javac -d bin -cp src;libs/jssc.jar src\SubMCUTester.java
javac -d bin -cp src;libs/jssc.jar src\BdosSerialSender.java
javac -d bin -cp src;libs/jssc.jar src\TelnetSender.java
javac -d bin -cp src;libs/jssc.jar src\ESP8266Tester.java
