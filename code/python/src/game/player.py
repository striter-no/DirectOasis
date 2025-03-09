from src.game.scene import *
from src.game.scene import _key

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
        if keyboard.isKeyPressed(_key("w")) and scene.inBounds(self.x, self.y - 1):
            self.y -= 1
        
        elif keyboard.isKeyPressed(_key("s")) and scene.inBounds(self.x, self.y + 1):
            self.y += 1

        if keyboard.isKeyPressed(_key("a")) and scene.inBounds(self.x - 1, self.y):
            self.x -= 1
        
        elif keyboard.isKeyPressed(_key("d")) and scene.inBounds(self.x + 1, self.y):
            self.x += 1

    def display(self, console: Console):
        console.pixel(
            self.x, self.y, clr.Fore.GREEN + self.simb, True
        )