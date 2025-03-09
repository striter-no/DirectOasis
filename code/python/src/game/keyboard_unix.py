from pynput import keyboard as _keyboard

def _key(char: str) -> _keyboard.KeyCode:
    return _keyboard.KeyCode.from_char(char)

class Kboard:
    def __init__(self):
        self.listener = _keyboard.Listener(on_press=self._on_press, on_release=self._on_release)
        self.pressed_keys = set()

    def start(self):
        self.listener.start()

    def stop(self):
        self.listener.stop()

    def _on_press(self, key):
        self.pressed_keys.add(key)

    def _on_release(self, key):
        if key in self.pressed_keys:
            self.pressed_keys.remove(key)

    def isKeyPressed(self, key: str) -> bool:
        return _key(key) in self.pressed_keys
