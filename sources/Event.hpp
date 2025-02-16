#ifndef TR_EVENT_HPP
#define TR_EVENT_HPP

TR_BEGIN_NAMESPACE()

struct Event {
  double currentTime = 0.0;
  double elapsedTime = 0.0;
};

struct MouseEvent: public Event {
  double x = 0.0, y = 0.0;
  bool pressed = false;;
};

struct ScrollEvent: public Event {
  double xOffset = 0.0;
  double yOffset = 0.0;
};

// TODO: KISS but...
struct KeyboardEvent: public Event {
  bool keyA = false;
  bool keyD = false;
  bool keyS = false;
  bool keyW = false;
  bool shift = false;
  bool space = false;
};

TR_END_NAMESPACE()

#endif // TR_EVENT_HPP
