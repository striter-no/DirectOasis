from src.game.scene import *

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