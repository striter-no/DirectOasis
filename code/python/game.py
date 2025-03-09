from src.game.netprot.client import *
from src.game.game import *
from random import *
import time
import multiprocessing

scene = Scene(60, 30)
player = Player(randint(0, scene.data.width - 1), randint(0, scene.data.height - 1), '@')
manager = multiprocessing.Manager()
entities = []# manager.list()
ent_lock = multiprocessing.Lock()

def get_others_poses():
    entities = {}
    broadcasts = client.get_all("broadcast")
    for broadcast in broadcasts:
        entities[broadcast.pub_uid] = broadcast.content.split(':')
    
    positions = []
    for entity in entities:
        positions.append((int(entities[entity][0]), int(entities[entity][1])))
    
    return positions

def send_my_position(position):
    message = f"{position[0]}:{position[1]}"
    client.broadcast(message, "text")

def text(x:int, y: int, text: str, console: Console):
    for i, char in enumerate(text):
        console.pixel(i + x, y, char, True)

def net_update():
    while True:
        with ent_lock:
            # print(">> Updating entities")
            global entities
            send_my_position([player.x, player.y])
            # print("... Sent my position")
            entities = get_others_poses()
            # print("... Got others' positions")
            time.sleep(0.06)

def draw(tick: int, console: Console, keyboard: Keyboard, mouse: Mouse):
    player.update(keyboard, scene)
    scene.display(console)

    with ent_lock:
        for other in entities:
            console.pixel(other[0], other[1], Fore.RED + '@', True)
    
    player.display(console)

if __name__ == "__main__":
    terminal = Terminal()
    console  = Console(
        terminal.width,
        terminal.height,
        " "
    )
    keyboard, mouse = Keyboard(), Mouse()
    keyboard.start()
    mouse.start()

    client = GameClient(
        "127.0.0.1", 
        8080, 
        "bob" + str(randint(0, 100)),
        logging=False
    )
    client.connect()
    client.registrate()
    process = multiprocessing.Process(target=net_update, args=())
    process.start()
    # threading.Thread(target=net_update, args=(), daemon=True).start()

    try:
    # if True:
        tick = 0
        while True:
            console.clear()

            draw(tick, console, keyboard, mouse)
            text(scene.width + 1, 1, f"My Position: {player.x} {player.y}", console)
            text(scene.width + 1, 3, f"My name: {client.nickname}", console)

            console.draw()
            time.sleep(0.03)
            tick += 1
    except:
        pass

    keyboard.stop()
    mouse.stop()
    console.showCursor()
    client.disconnect()