
#include "MFRC522.h"
#include <LiquidCrystal_I2C.h>

#include <Wire.h>

#define RST_PIN  5 // RST-PIN for RC522 - RFID - SPI - Modul GPIO5 
#define SS_PIN  4 // SDA-PIN for RC522 - RFID - SPI - Modul GPIO4
#define buzzer 15// buzzer
#define greenled 10

#include <ESP8266WiFi.h>           // Use this for WiFi instead of Ethernet.h
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); 
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
IPAddress server_addr(192,168,0,3);  // IP of the MySQL *server* here
char user[] = "UserName Here";              // MySQL user login username
char password[] = "Password Here";        // MySQL user login password
String UIDHex = "";
char UID[20];
// Sample query
//char INSERT_SQL[60];
char query[200];
//char compareUID_query[100];
char jehad[]= "('Jehad Oumer')";
//char finall[]= String(INSERT_SQL);
// WiFi card example
char ssid[] = "WIFI SSID HERE";         // your SSID
char pass[] = "WIFI PASSWORD HERE";     // your SSID Password
int lcdcount=0;
WiFiClient client;                 // Use this for WiFi instead of EthernetClient
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;

void setup()
{
  pinMode(buzzer,OUTPUT);
  pinMode(greenled,OUTPUT);
 // pinMode(yellowled,OUTPUT);
  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522
  //Serial.begin(115200);

//Use predefined PINS consts

  Wire.begin(D4, D3);

  lcd.begin();

  lcd.home();
  Serial.begin(115200);
  //while (!Serial); // wait for serial port to connect. Needed for Leonardo only
  
  // Begin WiFi section
  Serial.printf("\nConnecting to %s", ssid);
  lcd.print("Connecting");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(greenled,HIGH);
    delay(500);
    digitalWrite(greenled,LOW);
    delay(500);
    digitalWrite(greenled,HIGH);
    delay(500);
    digitalWrite(greenled,LOW);
    Serial.print(".");
    lcdcount++;
    lcd.setCursor(lcdcount,1);
    lcd.print(".");
  }

  if (WiFi.status() == WL_CONNECTED){
    lcd.clear();
    lcd.print("Connected");
    digitalWrite(greenled,LOW);
    delay(1000);
   
    
  }
  

  // print out info about the connection:
  Serial.println("\nConnected to network");
  Serial.print("My IP address is: ");
  Serial.println(WiFi.localIP());

  Serial.print("Connecting to SQL...  ");
  lcd.clear();
  lcd.print(" SQL...  ");
  if (conn.connect(server_addr, 3306, user, password)){
    
    lcd.setCursor(0,1);
    lcd.print("OK.");
    Serial.println("OK.");
    delay(1000);
    lcd.clear();
    lcd.print("Please Place");
    lcd.setCursor(0,1);
    lcd.print("Your Card");
  }
  else{
    Serial.println("FAILED.");
    lcd.print("FAILED.");}
  
  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);
}

void loop()
{
    //Serial.print(UIDHex);
  //UIDHex = "";
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    Serial.println("bro");
    
    delay(50);
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    Serial.println("dudddde");
    delay(50);
    return;
  }
//  UIDHex = "";
//    Serial.print(F("Card UID:"));
//    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
//    compareUID();
//    Insert();
  // Show some details of the PICC (that is: the tag/card)
//  Serial.print(F("Card UID:"));
//  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
//  Serial.println();
  
  if (conn.connected()){
    UIDHex = "";
    Serial.println("dddd");
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    variables_Setup();
    Insert();
//    Serial.println();
//    sprintf(query, "SET @mentor_Name= (SELECT Name FROM inspire.Mentors WHERE UID  ='%s');",UID  );
      //sprintf(query, "SET @mentor_Name= (SELECT Name FROM inspire.mentors WHERE UID  ='%s')",UID  );
//    Serial.println(query);
    //sprintf(query, "INSERT INTO inspire.test (message) VALUES %s", jehad);
      //cursor->executemany("SET @mentor_Name= (SELECT Name FROM inspire.Mentors WHERE UID  ='66706cac');");
      //Serial.println(query);
//    Serial.println("Done");
  }
}
  
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
   UIDHex = UIDHex + String(buffer[i],HEX);
    //Serial.print(buffer[i] < 0x10 ? " 0" : " ");
   Serial.println("UID IS %s ", UIDHex);
    //Serial.begin(115200);

     //Use predefined PINS consts
   
  }
  
  UIDHex.toCharArray(UID,20);
  //digitalWrite(greenled,LOW);
  digitalWrite(buzzer,HIGH);
  delay(200);
  digitalWrite(buzzer,LOW);
  lcd.clear();
  lcd.print("UID: ");
  lcd.print(UID);
  lcd.setCursor(0,1);
  lcd.print("Please Wait...");
  Serial.println(UID);
}
void variables_Setup(){
Serial.println(query);
sprintf(query, "SET @mentor_Name= (SELECT Name FROM inspire.mentors WHERE UID  ='%s')",UID  );
Serial.println(query);
cursor->execute(query);
Serial.println("Done");

strcpy(query,"SET @date= replace(CURRENT_DATE, '-','')");
  //query = "SET @date= replace(CURRENT_DATE, '-','');";
  Serial.println(query);
  cursor->execute(query);
  Serial.println("Done");
//
  strcpy(query,"SET @datecombine :=CONCAT('CREATE TABLE IF NOT EXISTS inspire.Entry',@date, '( Name char(20) NOT NULL PRIMARY KEY, UID char(20),Intime char(20), Outtime char(20) )')");
  //query = "SET @datecombine :=CONCAT('CREATE TABLE IF NOT EXIST Entry',@date, '( Name char(20), UID char(20),Intime TIMESTAMP, Outtime TIMESTAMP);";
  Serial.println(query);
  cursor->execute(query);
  Serial.println("Done");
//
  strcpy(query,"PREPARE today_Table FROM @datecombine");
  //query="PREPARE today_Table FROM @datecombine;";
  Serial.println(query);
  cursor->execute(query);
  Serial.println("Done");

Serial.println("UID IS %s ", UIDHex);

}

void Insert()
{
  
//
  strcpy(query,"EXECUTE today_Table");
  //query="EXECUTE today_Table;";
  Serial.println(query);
  cursor->execute(query);
  Serial.println("Done");



  strcpy(query,"SET @record :=CONCAT('update inspire.entry',@date,' set outtime=current_time where intime is not null')");
  Serial.println(query);
  cursor->execute(query);
  Serial.println("Done");

  strcpy(query,"PREPARE insert_record FROM @record");
  Serial.println(query);
  cursor->execute(query);
  Serial.println("Done");

  strcpy(query,"EXECUTE insert_record");
  Serial.println(query);
  cursor->execute(query);
  Serial.println("Done");

  
  strcpy(query,"SET @record :=CONCAT('insert into inspire.entry',@date, '(Name,UID,Intime) select (SELECT Name FROM inspire.Mentors WHERE Name=@mentor_Name),(SELECT UID FROM inspire.Mentors WHERE Name=@mentor_Name),(SELECT current_time)')");
  Serial.println(query);
  cursor->execute(query);
  Serial.println("Done");

  strcpy(query,"PREPARE insert_record FROM @record");
  Serial.println(query);
  cursor->execute(query);
  Serial.println("Done");

  strcpy(query,"EXECUTE insert_record");
  Serial.println(query);
  cursor->execute(query);
  Serial.println("Done");


  digitalWrite(buzzer,HIGH);
  delay(200);
  digitalWrite(buzzer,LOW);
  delay(50);
  digitalWrite(buzzer,HIGH);
  delay(200);
  digitalWrite(buzzer,LOW);
  digitalWrite(greenled,HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Done Registering");
  lcd.setCursor(0,1);
  lcd.print("Thank You");
  delay(5000);
  digitalWrite(greenled,LOW);
  lcd.clear();
  lcd.print("Please Place");
  lcd.setCursor(0,1);
  lcd.print("Your Card");
 // 
  Serial.println("UID IS %c ", UIDHex);
  //sprintf(query, "SET @mentor_Name= (SELECT Name FROM inspire.Mentors WHERE UID  ='%s');",UID  );
  //INSERT INTO inspire.Entry20180530 (Name,UID)SELECT Name,UID FROM inspire.Mentors WHERE Name=@mentor_Name;

  
  
}



