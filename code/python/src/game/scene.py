from src.capi import *
from src.capi import _keyboard
from src.capi import colorama as clr

class Scene:
    def __init__(
        self,
        width: int,
        height: int,
    ):
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
