#include <ArxContainer.h>
#include <Arduino.h>
#include <FastLED.h> // Библа Светодиодов
CRGB st[3][ST_NUMLED_MST]; // Жирная скотина. Много не ставить
CLEDController *stcont[3];

class ledSetting{
    private:
    int _st;
    byte _brightness = 0;
    struct _groupParmsSTR {
        int _numMin;
       int _numMax;
        byte _color[3];
       byte _param[2];
        bool switcher = 1;
int i = _numMin;
       
    };
arx::vector<struct _groupParmsSTR> _groupParmsVEC ;

    

    public:
    ledSetting(int st){
        _st = st;
    }
    void addGroup(int id, int numMin, int numMax){
        if(numMin >= 0){
       // _groupParmsVEC.push_back({numMin, numMax});
        _groupParmsVEC[id]._numMin = numMin;
        _groupParmsVEC[id]._numMax = numMax;
        }

    }
    void setMode(int id, int set1, int set2){
        _groupParmsVEC[id]._param[0] = set1;
        _groupParmsVEC[id]._param[1] = set2;

    }
    void setColor(int id, byte R, byte G, byte B){
        _groupParmsVEC[id]._color[0] = R;
        _groupParmsVEC[id]._color[1] = G;
        _groupParmsVEC[id]._color[2] = B;
    }
    void setBrightness(byte brightness){
        _brightness = brightness;
    }
    
    void work(int id){
        switch (_groupParmsVEC[id]._param[0])
        {
            // RS
        case 0:
            break;
            // STC
        case 1:{
            st[_st][_groupParmsVEC[id].i].setRGB(_groupParmsVEC[id]._color[0], _groupParmsVEC[id]._color[1], _groupParmsVEC[id]._color[2]);
            stcont[_st]->showLeds(_brightness);
            _groupParmsVEC[id].i++;
            if( _groupParmsVEC[id].i > _groupParmsVEC[id]._numMax) _groupParmsVEC[id].i = _groupParmsVEC[id]._numMin;
        }
            break;
            // RnS
        case 2:{
             
           
            if(_groupParmsVEC[id].switcher){
                st[_st][_groupParmsVEC[id].i].setRGB(_groupParmsVEC[id]._color[0], _groupParmsVEC[id]._color[1], _groupParmsVEC[id]._color[2]);
                stcont[_st]->showLeds(_brightness);
                _groupParmsVEC[id].i++;
                if( _groupParmsVEC[id].i > _groupParmsVEC[id]._numMax) {
                    _groupParmsVEC[id].i = _groupParmsVEC[id]._numMin;
                    _groupParmsVEC[id].switcher = !_groupParmsVEC[id].switcher;
                }

            }else{
                st[_st][_groupParmsVEC[id].i].setRGB(0, 0, 0);
                 stcont[_st]->showLeds(_brightness);
                _groupParmsVEC[id].i++;
                if( _groupParmsVEC[id].i > _groupParmsVEC[id]._numMax) {
                    _groupParmsVEC[id].i = _groupParmsVEC[id]._numMin;
                    _groupParmsVEC[id].switcher = !_groupParmsVEC[id].switcher;
                }
            }


        }
        break;
            // LM
        case 3:
            break;
            // LMR
        case 4:
            break;
            // LMC
        case 5:
            break;
            // TF
        case 6:
            break;
            // TS
        case 7:
            break;
        }
    }
};




