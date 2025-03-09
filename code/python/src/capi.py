from enum import Enum
import colorama
import termios
import select
import sys
import tty
import os
colorama.init()
import platform


class Keyboard:
    def __init__(self):
        if platform.system() == 'Windows':
            import src.game.keyboard_win as internal_kb
            # print("WINDOWS")
        else:
            import src.game.keyboard_unix as internal_kb
            # print("UNIX")
        self.instanse = internal_kb.Kboard()
    
    def start(self):
        self.instanse.start()
    
    def stop(self):
        self.instanse.stop()
    
    def isKeyPressed(self, key: str):
        return self.instanse.isKeyPressed(key)
    
    def isKeyReleased(self, key: str):
        raise NotImplementedError("Method isKeyReleased is not implemented")

class MouseButton(Enum):
    LEFT = "left"
    RIGHT = "right"
    MIDDLE = "middle"
    UNKNOWN = "unknown"

class Terminal:
    def __init__(self):
        self.width, self.height = os.get_terminal_size()

class Console:
    def __init__(self, width: int, height: int, backChar: str = " ", backGroundPref: str = ""):
        self.width = width
        self.height = height
        self.backChar = backChar

        self.background = backGroundPref
        self.screen = [[self.backChar for x in range(self.width)] for y in range(self.height)]

    def clear(self):
        self.screen = [[self.backChar for x in range(self.width)] for y in range(self.height)]
        print("\033[H\033[2J", end="") 
    
    def pixel(self, x: int, y: int, char: str, isColored: bool = False):
        if x < 0 or x >= self.width or y < 0 or y >= self.height:
            return
        self.screen[round(y)][round(x)] = char + (colorama.Fore.RESET if isColored else "")

    def draw(self):
        print(self.background + "\n".join(["".join(line) for line in self.screen]), end="")

    def hideCursor(self):
        print("\033[?25l", end="")
    
    def showCursor(self):
        print("\033[?25h", end="")

class Mouse:
    def __init__(self):
        self.x = 0
        self.y = 0
        self.pressed_button = MouseButton.UNKNOWN
        self._old_settings = None
        
        # Включаем расширенное отслеживание мыши (включая перемещение)
        sys.stdout.write("\033[?1003h")  # Все события мыши
        sys.stdout.flush()

    def start(self):
        self._old_settings = termios.tcgetattr(sys.stdin)
        tty.setcbreak(sys.stdin.fileno())

    def stop(self):
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, self._old_settings)
        sys.stdout.write("\033[?1003l")  # Выключаем отслеживание
        sys.stdout.flush()

    def poll_event(self) -> bool:
        """Обрабатывает все события в буфере"""
        processed = False
        while self._has_input():
            # Читаем escape-последовательность (6 символов: \033[M <b> <x> <y>)
            seq = sys.stdin.read(3)
            if seq != "\033[M":
                # Пропускаем неизвестные последовательности
                self._flush_input()
                continue
                
            # Читаем данные события
            data = sys.stdin.read(3)
            if len(data) != 3:
                break
                
            b, cx, cy = data
            button_code = ord(b)
            
            # Декодируем координаты (XTerm-формат)
            self.x = ord(cx) - 32  # Начинаются с ASCII 32 (' ')
            self.y = ord(cy) - 32
            
            # Определяем кнопку
            if button_code & 0b10000000:
                # Кнопка отпущена
                self.pressed_button = MouseButton.UNKNOWN
            else:
                # Определяем нажатую кнопку
                if button_code & 0b00000011 != 0:
                    self.pressed_button = MouseButton.LEFT
                elif button_code & 0b00000100 != 0:
                    self.pressed_button = MouseButton.RIGHT
                elif button_code & 0b00001000 != 0:
                    self.pressed_button = MouseButton.MIDDLE
            
            processed = True
        
        return processed

    def _has_input(self):
        return select.select([sys.stdin], [], [], 0)[0]

    def _flush_input(self):
        while self._has_input():
            sys.stdin.read(1)

    def get_position(self) -> tuple[int, int]:
        return (self.x, self.y)

    def is_button_pressed(self, button: MouseButton) -> bool:
        return self.pressed_button == button

class Buffer:
    def __init__(
        self,
        width: int,
        height: int,
        background: str,
        default: str = " ",
        lines: str = "", 
        lines_list: list[str] = []
    ):
        self.width = width
        self.height = height
        self.background = background

        self.data = lines_list if not len(lines_list) == 0 else (lines.split("\n") if lines != "" else [default * width for i in range(height)])

    def get(self, x: int, y: int):
        if x < 0 or y < 0 or x >= self.width or y >= self.height:
            return None
        return self.data[y][x]

    def _comp_lines(self):
        return "\n".join(self.data)

    def display_buffer(self, x: int, y: int, console: Console):
        for _y in range(len(self.data)):
            for _x in range(len(self.data[0])):
                px = self.get(_x, _y)
                console.pixel(
                    x + _x,
                    y + _y,
                    self.background + ('' if px is None else px)
                )

    def inject_to(self, buffer_dst, x, y):
        lines_dst = buffer_dst.data.copy()

        for _y in range(len(lines_dst)):
            for _x in range(len(lines_dst[0])):
                px = self.get(_x, _y)
                if px:
                    lines_dst[_y + y][x + _x] = px

        return Buffer(self.comp_lines(lines_dst))