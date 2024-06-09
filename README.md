This is an Arduino Library to be able to communicate with the encoders currently on our Sailbot and PEP boats. \
To use this library, you must place the repository in a folder in your libraries folder in your Arduino folder. \
Your Arduino folder is generally located in your Documents folder so the full file path for this repository should be \
Documents/Arduino/libraries/<THIS_REPO>.

Once you have downloaded and placed this library in the correct place, you should be able to access the AMT22 Encoder by \
importing the library and calling the get_motor_angle() function on an AMT22_Encoder object.