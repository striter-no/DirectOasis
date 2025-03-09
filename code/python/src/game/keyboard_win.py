import keyboard
import threading
from typing import Dict, Optional

class Kboard:
    def __init__(self):
        self.pressed_keys: Dict[str, Optional[str]] = {}
        self._lock = threading.Lock()
        self._stop_event = threading.Event()
        self._thread = threading.Thread(target=self._listen, daemon=True)
        
    def start(self) -> None:
        """Запускает поток отслеживания клавиатуры"""
        self._thread.start()
        
    def stop(self) -> None:
        """Останавливает поток отслеживания"""
        self._stop_event.set()
        self._thread.join()
        
    def _listen(self) -> None:
        """Внутренний метод для прослушивания событий"""
        while not self._stop_event.is_set():
            event = keyboard.read_event(suppress=True)
            with self._lock:
                if event.event_type == keyboard.KEY_DOWN:
                    # Получаем Unicode символ или название клавиши
                    char = event.name if len(event.name) == 1 else None
                    self.pressed_keys[event.scan_code] = char
                elif event.event_type == keyboard.KEY_UP:
                    self.pressed_keys.pop(event.scan_code, None)
    
    def isKeyPressed(self, key: str) -> bool:
        """Проверяет, нажата ли клавиша по имени (e.g. 'a', 'space')"""
        return keyboard.is_pressed(key)
    
    def get_unicode_chars(self) -> Dict[int, Optional[str]]:
        """Возвращает словарь нажатых Unicode-символов"""
        with self._lock:
            return {k: v for k, v in self.pressed_keys.items() if v is not None}
    
    def get_raw_codes(self) -> Dict[int, Optional[str]]:
        """Возвращает все нажатые скан-коды"""
        with self._lock:
            return self.pressed_keys.copy()