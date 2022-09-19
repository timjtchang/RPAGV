/*
 *  Copyright © 2022 Realplus Tech. by Tim J.
 *  The main code for RP_AGV
 *  Using ATMega2560
 *  
 *  Command table
 *  
 *      QQQA????XXX
 *      [3] Alphabet
 *      [4:7] number
 *  
 *  Command Frequency
 *  D > Z > P > A > B > C > J > 
 *  
 */


char current_location[] = "A0000";  //目前位置
char target_location[] = "A0000";   //目標位置
char RFID_code[] = "A0000";         //最近的RFID卡號

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);

}

void loop() {

    char cmd[] = "QQQJ0020XXX";         //讀取指令
    char card_number[] = "0000";       //讀取卡號
    char report[] = "QQQ00000XXX";      //回報指令

    if( cmd[3] == 'D' ) DCmd( cmd );
    else if( cmd[3] == 'Z' ) ZCmd( cmd );
    else if( cmd[3] == 'P' ) PCmd( cmd );
    else if( cmd[3] == 'A' ) for( int i=4 ; i<=7 ; i++ )card_number[i-4] = cmd[i];
    else if( cmd[3] == 'B' ) BCmd( cmd, report, card_number );
    else if( cmd[3] == 'C' ) CCmd( cmd, report, card_number );
    else if( cmd[3] == 'J' ) JCmd( cmd, current_location, target_location, RFID_code );
    else return;

    Serial.print( "card_number= " );
    Serial.println( card_number );
    
    Serial.print( "report= " );
    Serial.println( report );

    Serial.print( "current_location= " );
    Serial.println( current_location );

    Serial.print( "target_location= " );
    Serial.println( target_location );

    Serial.print( "RFID= " );
    Serial.println( RFID_code );
    
}

void BCmd( char* cmd, char* report, char* card ){

    uint8_t speed = cmd[7]-'0';

    for( int i=0 ; cmd[i]!='\0' ; i++ )report[i] = cmd[i];
    for( int i=4 ; i<=6 ; i++ ) card[i-3] = cmd[i];
    
    D_speed( speed )? report[7] = '1' : report[7] = '0';
    
}

/*
 *  指令C
 */

void CCmd( char* cmd, char* report, char* card ){
    
    uint8_t direction = cmd[7]-'0';

    for( int i=0 ; cmd[i]!='\0' ; i++ )report[i] = cmd[i];
    for( int i=4 ; i<=6 ; i++ ) card[i-3] = cmd[i];
    
    D_direction( direction )? report[7] = '1' : report[7] = '0';
    
}

/*
 *  指令D
 */

void DCmd( char* cmd ){ // 

    String battery_report = "QQQL0000XXX";

    if( cmd[5] == '0' ){

        if( cmd[6] == '1' ) D_followLine( cmd[7]-'0' );
        else if ( cmd[6] == '2' ) D_sound( cmd[7]-'0' );
        else if ( cmd[6] == '5' ) D_direction( cmd[7]-'0' );
        else if ( cmd[6] == '7' ) D_speed( cmd[7]-'0' );
        else if ( cmd[6] == '6' ){
            
            uint8_t battery;
            battery = D_battery();
        
            battery_report[7] = battery%10+'0';
            battery_report[6] = (battery/10)%10+'0';
            battery_report[5] = (battery/100)%10+'0';

            Serial.println( "battery="+battery_report );
        }
        else return;
        
    }else if ( cmd[5] == '1' )D_stopFollow( cmd[7]-'0' );
    else return;
   
    
}

/*
 *    D 指令的敘述區
 *    
 */
void D_followLine( uint8_t cmd ){
    
    // 循跡，cmd為執行速度 1~9
    Serial.println( "in followLine" );
    Serial.print( "cmd= " );
    Serial.println( cmd );
    
}
void D_sound( uint8_t cmd ){

    // 音量，cmd為音量大小 0~9
    Serial.println( " in sound " );
    Serial.println( cmd );
    Serial.print( "cmd= " );
    Serial.println( cmd );
}

uint8_t D_direction( uint8_t cmd ){

    // 分岐走法，0為左、1為右、2為感測器決定
    Serial.println( " in direction " );
    Serial.print( "cmd= " );
    Serial.println( cmd );

    return 1; // if success
}
uint8_t D_battery(){
    
    // 電量百分比
    uint8_t battery = 0;
    Serial.println( "in battery ");

    // battery = 電量
    return battery;
    
}
uint8_t D_speed( uint8_t cmd ){  // 改變行進速度
    //TODO

    Serial.println( "D_speed");
    Serial.print( "cmd= " );
    Serial.println( cmd );

    return 1; // if success

}

void D_stopFollow( uint8_t cmd){ // 停止循跡
    //TODO

    Serial.println( "stop follow ");
    if( cmd  == 1 ) Serial.println( "STOP" );
    else if( cmd == 2 ) Serial.println( "PAUSE");
    else return;
}

/*
 *  指令J
 */

void JCmd( char* cmd, char* current_ptr, char* target_ptr, char* RFID_ptr ){

    if( cmd[5] == '0' ){

        if( cmd[6] == '1' ){

            //申請路權
        }else if( cmd[6] == '2' ){

            //取得當前、目標位置
            char current_location[] = "B1234";// current_location = 當前位置
            char target_location[] = "C5678";// target_location = 目標位置

            for( int i=0 ; current_location[i]!='\0' ; i++){

                current_ptr[i] = current_location[i];
                target_ptr[i] = target_location[i];
            }
            
        }else if( cmd[6] == '3' ){

            //取得最後一次讀取的RFID
            char RFID_code[] = "D0987";// RFID_code = RFID值

            for( int i=0 ; RFID_code[i]!='\0' ; i++ ) RFID_ptr[i] = RFID_code[i];
     
        }else if( cmd[6] == '4' ){

            //取得電量
            D_battery();
            
        }else return;
        
    }else if( cmd[5] == '1' ){

        if( cmd[6] == '3' ){

            //取消任務
        }
        
    }else return;
    
    
}

void PCmd( char* cmd ){

    uint8_t song;               // 曲目
    uint8_t performance;        // 表現方式

    song = (cmd[4]-'0')*100+(cmd[5]-'0')*10+(cmd[6]-'0');
    performance = cmd[7]-'0';

    // playSong( song );
    Serial.print( "song= " );
    Serial.println( song );
    
    // 播放方式
    if( performance==0 ){ // 停止

        Serial.println( "stop music" );
        
    }else if( performance==1 ){ // 播放曲目，不等待曲目是否播完

        Serial.println( "play music" );
   
    }else if( performance==2){ // 播放曲目，並等待曲目整首播完

        Serial.println( "play music until end" );
        
    }else if( performance==3 ){ // 重復播放曲目

        Serial.println( "replay music" );

    }else if( performance==4 ){ // 播放曲目，並設為背景音

        Serial.println( "play music and set as background music" );
        
    }else if( performance==5 ){ // ：停止背景音

        Serial.println( "stop background music" );
    }

    return;
}

/*
 *  指令Z
 */

 void ZCmd( char* cmd ){

    if( cmd[6] == '1' ) backTime( cmd[7]-'0');
    else if( cmd[6] == '2' ) turnAround_right( cmd[7]-'0' );
    else if( cmd[6] == '3' ) turnAround_left( cmd[7]-'0' );
    else if( cmd[6] == '4' ) waitButton( cmd[7]-'0' );
    else if ( cmd[6] == '5' ) (cmd[7]-'0')? delay( (cmd[7]-'0')*1000 ): delay( 1000 );
    else if( cmd[6] == '6' ) (cmd[7]-'0')? delay( (cmd[7]-'0')*10000 ): delay( 10000 );

    return;
    
 }
/*
 *  Z指令敘述區
 */

 void backTime( uint8_t n ){ // 後退 n*1S 

    Serial.println( "backTime" );
    Serial.println( n );
    return;
 }

 void turnAround_right( uint8_t n ){ 

    if( !n ){ // 右迴旋 

        Serial.println( "turn right around" );
        Serial.println( n );
        
    }else{    // 右迴旋後循跡 

        Serial.println( "turn right around and follow" );
        Serial.println( n );
        
    }
    
 }

 void turnAround_left( uint8_t n){

    if( !n ){   // 左迴旋

        Serial.println( "turn left around" );
        Serial.println( n );

    }else{      // 左迴旋後循跡
        
        Serial.println( "turn left around and follow" );
        Serial.println( n );
        
    }
 }

void waitButton( uint8_t n ){

    if( n==1 ){ // 等 Run Button 按下

        Serial.println( "wait run button " );
        Serial.println( n ); 
           
    }else if( n==2 ){ // 等Err Button 按下
        
        Serial.println( "wait erro button " );
        Serial.println( n ); 

    }else if( n==3 ){ // 等 Green Button 按下

        Serial.println( "wait green button " );
        Serial.println( n );
        
    }else if( n==4 ){ // 等 Red Button 按下

        Serial.println( "wait red button " );
        Serial.println( n );
        
    }

    return;
 }
