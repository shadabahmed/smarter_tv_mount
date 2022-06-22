#ifndef REMOTE_H

#define REMOTE_H

#define BUTTON_D0 10
#define BUTTON_D1 11
#define BUTTON_D2 12
#define BUTTON_D3 16

#define BUTTON_UP BUTTON_D0  
#define BUTTON_DOWN BUTTON_D1
#define BUTTON_LEFT BUTTON_D2
#define BUTTON_RIGHT BUTTON_D3

class Remote {
  public:
  Remote();
  bool isButtonPressed();
  int getButtonCode();
};

#endif
