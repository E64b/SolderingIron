/*

Soldering Iron - паяльный утюг!
Что может быть лучше нижнего подогрева? Нижний подогрев сделанный своими руками!
За основу взят нагревательный элемент в виде подошвы утюга, в роли датчика температуры MAX6675 с термопарой.
Управляет нагрузкой семистор.
Дисплей двухстрочный подключенный по шине I2C
Контроллер Atmega328P
by e64b

*/




using namespace std;
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include <EEPROM.h>
#include <GyverMAX6675.h>

float Temp = 0;
bool Healer;

struct Data
{
    float SetTemp = 0;
};

#define Left digitalRead(4)
#define Right digitalRead(3)
#define CLK_PIN   13  // Пин SCK
#define DATA_PIN  12  // Пин SO
#define CS_PIN    10  // Пин CS

uint32_t display;

LiquidCrystal_I2C lcd(0x27, 16, 2);
GyverMAX6675<CLK_PIN, DATA_PIN, CS_PIN> sens;

Data data;

void setup()
{
    Serial.begin(9600);

    //EEPROM.put(0, data); //Прошивка переменных в еепром при первой компиляции раскоментировать << прошить прошивку в Ардуино, закоментировать и прошить снова без этой строки

    lcd.init();
    lcd.backlight();
    lcd.home();
    lcd.print("Soldering iron");

    pinMode(3, INPUT_PULLUP); //Кнопка Right
    pinMode(4, INPUT_PULLUP); //Кнопка Left
    pinMode(6, OUTPUT); //Нагреватель

    EEPROM.get(0, data);

    delay(1000);
    lcd.clear();
}

void loop()
{
    
    Temp = sens.getTemp(); //Считывание температуры
    digitalWrite(6, Healer); //Управление нагревателем
 
  /*Раз в 2 секунды текущая и выставленная выводятся на дисплей*/
  if  ((millis() - display >= 2000) or (Left == LOW) or (Right == LOW)) 
  {
    display = millis();
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Temp");  lcd.setCursor(5, 0);  lcd.print(Temp);
    lcd.setCursor(0, 1); lcd.print("Set Temp"); lcd.setCursor(9, 1); lcd.print(data.SetTemp);
  }

  
/*Ести выставленная температура ниже текущей, включается нагреватель, если температура выше заданной то выключается*/
 if (data.SetTemp > Temp)
    {
        Healer = HIGH;
    }
    else
    {
        Healer = LOW;
    }

/*Убавить температуру*/
    if (Left == LOW)
    {
        data.SetTemp--;
    }

/*Прибавить температуру*/
    if (Right == LOW)
    {
        data.SetTemp++;
    }

/*При одновременном нажатии кнопки Влево и Вправо настройки сохраняются в память*/
    if ((Left == LOW) and (Right == LOW)) 
    {
        lcd.clear();
        lcd.home();
        EEPROM.put(0, data);
        lcd.print("Setting save");
        delay(1000);
    }


delay (1000);


}
