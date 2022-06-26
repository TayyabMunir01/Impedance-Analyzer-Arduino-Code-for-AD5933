#include<Wire.h>
#include<math.h>

int16_t  real1,real2,imag1,imag2;
double    real = 0;
double   imag = 0;
double    magnitude;
double    impedance;
//double    gain = 0.0000000002848;
//double    gain = 0.0000000049776;
//double gain = 0.000000001057;
double gain = 0.000000000546; //for 220k, works fine
//double gain = 0.000000131181; //for 10k, stuck gives 20k short
//double gain = 0.00000000498132; //for 10k again gives around 20075 magnitude
//double gain = 0.00000000060558; //for 100k again
//double gain = 0.00000000060789; //for 100k, less values
//double gain = 0.000000000542132; //for 150k, works fine
//double gain = 0.00000000052428; // for 880k or 920k real value
//double gain = 0.0000000011098; //for 47k gives 19.5k almost short
//double gain = 0.00000000068784; //for 82k
//double gain = 0.0000000120473; //for 10k with 220k factor in series
//double gain = 0.000000012255; //again for 10k with 220k factor in series
//double gain = 0.00019760479; //potentiometer in series with unknown 220k
//double gain = 0.00000000051282; //for 1M ohm, gives less value
//double gain = 0.00000000597; //for 10k with op amp
//double gain = 0.0000000307; //for capacitor 10 at position 18 doesnt work

int8_t   status_reg_val =0;

//registers
#define ctrl_reg1 0x80
#define ctrl_reg2 0x81

#define start_freq_reg1 0x82
#define start_freq_reg2 0x83
#define start_freq_reg3 0x84

#define freq_inc_reg1 0x85
#define freq_inc_reg2 0x86
#define freq_inc_reg3 0x87

#define no_inc_reg1 0x88
#define no_inc_reg2 0x89

#define set_cylce_reg1 0x8A
#define set_cylce_reg2 0x8B

#define status_reg 0x8F

#define real_data_reg1 0x94
#define real_data_reg2 0x95
#define imag_data_reg1 0x96
#define imag_data_reg2 0x97

// Control register 0x80
#define slave_addr 0x0D                //slave address
#define reset 0x10                     //0x81 register for reset and internal clock
#define standby_mode 0xB1             //here 1 means pga = x1 and 2.0 peak to peak voltage
#define init_start_freq 0x11
#define start_freq_sweep 0x21
#define inc_freq 0x31
#define repeat_freq 0x41
#define power_down 0xA1

//reading pointer command
#define ptr_cmd 0xB0

void setup() {

  Wire.begin();
  Serial.begin(9600);                                                                                                                                                                                      

  
  // for start frequency =30kHz  
  Wire.beginTransmission(slave_addr);
  Wire.write(start_freq_reg1);
  Wire.write(0x05);
  Wire.endTransmission(); 
  Wire.beginTransmission(slave_addr);
  Wire.write(start_freq_reg2);
  Wire.write(0x1E);
  Wire.endTransmission(); 
  Wire.beginTransmission(slave_addr);
  Wire.write(start_freq_reg3);
  Wire.write(0xB8);
  Wire.endTransmission(); 
  // for frequency increment = 10Hz
  Wire.beginTransmission(slave_addr);
  Wire.write(freq_inc_reg1);
  Wire.write(0x00);
  Wire.endTransmission(); 
  Wire.beginTransmission(slave_addr);
  Wire.write(freq_inc_reg2);
  Wire.write(0x01);
  Wire.endTransmission(); 
  Wire.beginTransmission(slave_addr);
  Wire.write(freq_inc_reg3);
  Wire.write(0x4F);
  Wire.endTransmission(); 
  // for number of increment = 100
  Wire.beginTransmission(slave_addr);
  Wire.write(no_inc_reg1);
  Wire.write(0x00);
  Wire.endTransmission(); 
  Wire.beginTransmission(slave_addr);
  Wire.write(no_inc_reg2);
  Wire.write(0x0A);
  Wire.endTransmission(); 
   // for settling time 100 times 2
  Wire.beginTransmission(slave_addr);
  Wire.write(set_cylce_reg1);
  Wire.write(0b00000010);
  Wire.endTransmission(); 
  Wire.beginTransmission(slave_addr);
  Wire.write(set_cylce_reg2);
  Wire.write(0b01100100);
  Wire.endTransmission(); 




  //enter standby mode by setting the reset
  Wire.beginTransmission(slave_addr);
  Wire.write(ctrl_reg2);
  Wire.write(reset);
  Wire.endTransmission(); 
  //enter standby mode manually
  Wire.beginTransmission(slave_addr);
  Wire.write(ctrl_reg1);
  Wire.write(standby_mode);
  Wire.endTransmission(); 
  //initialize with start frequency
  Wire.beginTransmission(slave_addr);
  Wire.write(ctrl_reg1);
  Wire.write(init_start_freq);
  Wire.endTransmission();
  //start frequency sweep
  Wire.beginTransmission(slave_addr);
  Wire.write(ctrl_reg1);
  Wire.write(start_freq_sweep);
  Wire.endTransmission();


  
  Serial.println();
  Serial.println("Code Check 1 2 3");

  
//  again_read_value:
  delay(50);
  //now reading part ,checking bit D1 of register 0x8f, pointer to status regiter 8f
  Wire.beginTransmission(slave_addr);
  Wire.write(ptr_cmd);
  Wire.write(status_reg);
  Wire.endTransmission();
  
  again_read_value:
//  delay(100);
  //reading from the pointer/ the data register
  Wire.requestFrom(slave_addr,1,true);
  status_reg_val = Wire.read();
  Wire.endTransmission();
    
  //cheking if conversion is complete
  if((status_reg_val&0b00000010)>>1)
  {
    delay(500);
  Wire.beginTransmission(slave_addr);
  Wire.write(ptr_cmd);
  Wire.write(real_data_reg1);
  Wire.endTransmission();
  Wire.requestFrom(slave_addr,1,true);
  real1 = Wire.read();
  Wire.endTransmission();
  Wire.beginTransmission(slave_addr);
  Wire.write(ptr_cmd);
  Wire.write(real_data_reg2);
  Wire.endTransmission();
  Wire.requestFrom(slave_addr,1,true);
  real2 = Wire.read();
  Wire.endTransmission();

  Wire.beginTransmission(slave_addr);
  Wire.write(ptr_cmd);
  Wire.write(imag_data_reg1);
  Wire.endTransmission();
  Wire.requestFrom(slave_addr,1,true);
  imag1 = Wire.read();
  Wire.endTransmission();
  Wire.beginTransmission(slave_addr);
  Wire.write(ptr_cmd);
  Wire.write(imag_data_reg2);
  Wire.endTransmission();
  Wire.requestFrom(slave_addr,1,true);
  imag2 = Wire.read();
  Wire.endTransmission();
  
  real = (real1<<8) | real2;
  imag = (imag1<<8) | imag2;

  magnitude = sqrt(real*real + imag*imag);
  impedance = 1/(gain*magnitude);
    
  Serial.print("Impedance: ");
  Serial.print(impedance);
  Serial.print("    ");
  Serial.print("magnitude: "  );
  Serial.print(magnitude);
  Serial.print("    ");
  Serial.print("real: ");
  Serial.print(real);
  Serial.print("    ");
  Serial.print("Imaginary: ");
  Serial.println(imag);
//    Serial.println("imp: " + String(impedance) + "  mag: " + String(magnitude) + "  real: " + String(real) + "  imag: " + String(imag));
  }
  else
  {goto again_read_value;}

  

  //checking if sweep is complete
  Wire.beginTransmission(slave_addr);
  Wire.write(ptr_cmd);
  Wire.write(status_reg);
  Wire.endTransmission();
  Wire.requestFrom(slave_addr,1,true);
  status_reg_val = Wire.read();
  Wire.endTransmission();

  if (!((status_reg_val&0b00000100)>>2))
  {
  Wire.beginTransmission(slave_addr);
  Wire.write(ctrl_reg1);
  Wire.write(repeat_freq);
  Wire.endTransmission();

  goto again_read_value;
  }
  else
  {
  Wire.beginTransmission(slave_addr);
  Wire.write(ctrl_reg1);
  Wire.write(power_down);
  Wire.endTransmission();  
  Serial.println("sweep is complete");
  }
  
}

void loop() {

  
  
}
