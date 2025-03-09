from src.capi import *
from src.capi import Keyboard
from src.capi import colorama as clr

class Scene:
    def __init__(
        self,
        width: int,
        height: int,
    ):
        self.width = width
        self.height = height

        self.data = Buffer(
            width, height, "", "."
        )

    def inBounds(self, x: int, y: int) -> bool:
        return 0 <= x < self.data.width and 0 <= y < self.data.height

    def getPixelOn(self, x: int, y: int) -> str | None:
        return self.data.get(x, y)

    def display(self, console: Console):
        self.data.display_buffer(
            0, 0, console
        )

class Player:
    def __init__(
        self,
        x: int,
        y: int,
        simb: str
    ):
        self.x = x
        self.y = y
        self.simb = simb
    
    def update(self, keyboard: Keyboard, scene: Scene):
        if keyboard.isKeyPressed("w") and scene.inBounds(self.x, self.y - 1):
            self.y -= 1
        
        elif keyboard.isKeyPressed("s") and scene.inBounds(self.x, self.y + 1):
            self.y += 1

        if keyboard.isKeyPressed("a") and scene.inBounds(self.x - 1, self.y):
            self.x -= 1
        
        elif keyboard.isKeyPressed("d") and scene.inBounds(self.x + 1, self.y):
            self.x += 1

    def display(self, console: Console):
        console.pixel(
            self.x, self.y, clr.Fore.GREEN + self.simb, True
        )

class Entity:
    def __init__(
        self,
        x: int,
        y: int,
        simb: str
    ):
        self.x = x
        self.y = y
        self.simb = simb
    
    def display(self, console: Console):
        console.pixel(
            self.x, self.y, clr.Fore.RED + self.simb, True
        )