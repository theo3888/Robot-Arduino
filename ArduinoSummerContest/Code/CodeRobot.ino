/*================================================================Robot v2.4================================================================
 * 
 * Ecrit par Théo Nogues et commencé le 25/06/2016
 *     
 *
 * Principaux composants du robot :
 *  -> Controle de 2 moteurs avec Adafruit Motor Shield 
 *  -> 2 capteur à ultrasons SR-04 qui detectent les potentiels obstacles et font s'arrêter le robot
 *  -> Controle avec module bluetooth hc-05 via une application android créée sur AppInventor2 
 *  
 * A venir : 
 *   -> ?? Controle à l'aide d'un joystick avec AppInventor2 ?? 
 *   -> Controle via capteur infrarouge et telecommande quelconque (TV,Radio...) 
 *  
 * Alimentation du robot :
 *   -> PowerBank 2000mAh 5V (Batterie d'appoint pour smartphone) pour alimenter l'arduino et les composants
 *   -> Batterie LiPo 7.4V 1000mAh pour alimenter le shield moteur et les moteurs (ATTENTION, enlever le jumper VIN de la shield car 2 alims séparées !!!)
 *  
 *  
 *  Liens utiles :
 *  
 *  Adafruit Motor Shield
 *  http://www.robotshop.com/eu/fr/kit-blindage-controleur-moteur-arduino-v2.html
 *  
 *  Capteur à ultrasons SR-04
 *  http://www.robotshop.com/eu/fr/module-sonar-hc-sr04.html
 *  
 *  Module Bluetooth HC-05 :
 *  http://www.cdiscount.com/maison/bricolage-outillage/xcsource-bluetooth-serial-6pin-transceiver-emetteu/f-1170456-xcs4894479245408.html
 *  
 *  PowerBank 5V :
 *  http://www.boulanger.com/ref/8000530?xtor=SEC-1296-GOO&xts=171153&origin=%7Badtype%7D&kwd=%7Bkeyword%7D&gclid=CjwKEAjw_LG8BRDb1JTxm8uP_UwSJADu_8pWjdmGIgpBdGtNaQZRz9bE3nREWq4l1iHQy8_JWnqAPhoCZW7w_wcB
 *  
 *  Batterie 7.4V :
 *  http://www.breizh-modelisme.com/PBSCProduct.asp?ItmID=20843610&AccID=99195&PGFLngID=0&gclid=CjwKEAjw_LG8BRDb1JTxm8uP_UwSJADu_8pWPSqlrkLCkGMJ73WhNgyyp5bDIpoLI6U0gpiGid4NIRoCv2fw_wcB
 *  
 *  Site AppInventor 2 :
 *  http://ai2.appinventor.mit.edu
*/ 
                    
#include <SoftwareSerial.h>                              // Bibliothèque pour changer la voie serie du bluetooth                                             
#include <Adafruit_MotorShield.h>                        //
#include <Wire.h>                                        // Bibliothèque et fichiers du shield moteur
#include "utility/Adafruit_MS_PWMServoDriver.h"          //
 

#define bluetoothRX       3   // Broche RX                   > MODULE BLUETOOTH
#define bluetoothTX       4   // Broche TX                   > MODULE BLUETOOTH

#define trigPin1          6   // Broche Trig 1               > CAPTEUR A ULTRASONS
#define echoPin1          7   // Broche Echo 1               > CAPTEUR A ULTRASONS
#define trigPin2          8   // Broche Trig 2               > CAPTEUR A ULTRASONS
#define echoPin2          9   // Broche Echo 2               > CAPTEUR A ULTRASONS


 
                     // Variables :
char bluetoothData;               // Données envoyées par le module Bluetooth
int distance1, distance2;         // Distance mesurée par le capteur 1 et 2 (en cm)
long duree1, duree2;              // Durée de l'ultrason du capteur 1 et 2 
int limite = 50;                  // Limite de distance des capteurs (en cm)
int vitesseMoteurs = 60;          // Vitesse des moteurs /255


SoftwareSerial bluetooth (bluetoothTX, bluetoothRX); //Change les pins de la voie série

Adafruit_MotorShield AFMS = Adafruit_MotorShield();   // Initialise la shield moteur
 
Adafruit_DCMotor *moteurGauche = AFMS.getMotor(1);    // Le moteur 1 est le moteur gauche
Adafruit_DCMotor *moteurDroit = AFMS.getMotor(2);     // Le moteur 2 est le moteur droit


void setup() 
{
  pinMode(echoPin1, INPUT);  // La broche Echo du capteur 1 est une entrée
  pinMode(echoPin2, INPUT);  // La broche Echo du capteur 2 est une entrée
  pinMode(trigPin1, OUTPUT); // La broche Trig du capteur 1 est une sortie
  pinMode(trigPin2, OUTPUT); // La broche Trig du capteur 2 est une sortie

  AFMS.begin(); // Démarre la connexion avec la shield moteur

  moteurDroit->setSpeed(vitesseMoteurs);  // Défini la vitesse du moteur droit /255
  moteurGauche->setSpeed(vitesseMoteurs); // Défini la vitesse du moteur gauche /255
  
  bluetooth.begin(9600); // Démarre la voie série avec le module Bluetooth (9600bits/sec)
  
          
}

void loop() 
{
  
  while(bluetooth.available()==0)     // Si le module bluetooth ne reçoit pas de données, ne rien faire 
      {
      }
      
   while(bluetooth.available()>0)            // Si le module bluetooth reçoit des données, les stocker dans la variable bluetoothData
     {
      bluetoothData = bluetooth.read();
     }
     
          switch(bluetoothData)
          {
            case 0 : // Avancer quand le module bluetooth reçoit le chiffre 0 
            {
             capteur1();  // Appel des fonctions des capteurs à ultrasons
             capteur2();
             
                while (bluetooth.available()==0 && distance1 > limite && distance2 > limite )   // Tant que le module ne reçoit pas d'autres donnéees,
                {                                                                               // et que la distance mesurée par les capteurs est au dessus de la limite,
                  capteur1();                                                                   // le robot avance mais s'arrête quand une des condition n'est plus respectée
                  capteur2();
                  moteurDroit->run(FORWARD);  
                  moteurGauche->run(FORWARD);
              
                }
                
                moteurDroit -> run (RELEASE);
                moteurGauche -> run (RELEASE); 
                break; 
            }
            
            
            case 1 : // Reculer quand le module bluetooth reçoit le chiffre 1
            {
             moteurDroit -> run(BACKWARD);  // Le moteur droit recule
             moteurGauche -> run(BACKWARD); // Le moteur gauche recule
           
             break;
            }
        
          
            case 2 : // Aller à droite quand le module bluetooth reçoit le chiffre 2 
            {
             capteur1();  
             capteur2();
             
                while (bluetooth.available()==0 && distance1 > limite && distance2 > limite )  // Tant que le module ne reçoit pas d'autres donnéees,
                {                                                                              // et que la distance mesurée par les capteurs est au dessus de la limite,
                  capteur1();                                                                  // le robot avance mais s'arrête quand une des condition n'est plus respectée
                  capteur2();
                  moteurDroit->run(RELEASE);  
                  moteurGauche->run(FORWARD);
             
                }
                
                moteurDroit -> run (RELEASE);
                moteurGauche -> run (RELEASE); 
                break; 
            }
        
            
            case 3 : // Gauche quand le module bluetooth reçoit le chiffre 3 
            {
             capteur1();  
             capteur2();
             
                while (bluetooth.available()==0 && distance1 > limite && distance2 > limite ) // Tant que le module ne reçoit pas d'autres donnéees,
                {                                                                             // et que la distance mesurée par les capteurs est au dessus de la limite,
                  capteur1();                                                                 // le robot avance mais s'arrête quand une des condition n'est plus respectée
                  capteur2();
                  moteurDroit->run(FORWARD);  
                  moteurGauche->run(RELEASE);
           
                }
                
                moteurDroit -> run (RELEASE);
                moteurGauche -> run (RELEASE); 
                break; 
            }
           
            
            case 4 : // Stop quand le module bluetooth reçoit le chiffre 4
            {
             moteurDroit -> run (RELEASE); // Le moteur droit s'arrête
             moteurGauche -> run (RELEASE); // Le moteur gauche s'arrête
         
             break;
            }
           }
  }

int capteur1()    // Fonction du capteur ultrason 1
    {
      digitalWrite(trigPin1, LOW); 
      delayMicroseconds(2);             // trigPin1 niveau bas pendant 2µs
    
      digitalWrite(trigPin1, HIGH);
      delayMicroseconds(10);            // trigPin1 niveau haut pendant 10µs
      digitalWrite(trigPin1, LOW);
      
      duree1 = pulseIn(echoPin1, HIGH); // Retourne la durée du capteur 1
      distance1 = duree1*0.034/2;       // Calcule la distance du capteur 1 
    }
    
int capteur2()   // Fonction du capteur ultrason 1
    {
      digitalWrite(trigPin2, LOW);
      delayMicroseconds(2);               // trigPin2 niveau bas pendant 2µs
      digitalWrite(trigPin2, HIGH);
      delayMicroseconds(10);              // trigPin2 niveau haut pendant 10µs
      digitalWrite(trigPin2, LOW);
      
      duree2 = pulseIn(echoPin2, HIGH);   // Retourne la durée du capteur 2
      distance2 = duree2*0.034/2;         // Calcule la distance du capteur 2
    }
