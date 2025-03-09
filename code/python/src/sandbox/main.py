import sys

def audit_hook(event, args):
    # Блокируем системные вызовы и доступ к файлам
    if event in ('os.system', 'subprocess.Popen', 'open'):
        raise RuntimeError(f"Операция {event} запрещена в песочнице")

# Регистрируем audit hook
sys.addaudithook(audit_hook)

class Sandobx:
    def __init__(self):
        self.restricted_events = []

    def __hook(self, event, args):
        if event in self.restricted_events:
            raise RuntimeError(f"Operation {event} is not allowed in sandbox")

    def restrict_file_io(self):
        pass

    def new_restricted_func(self):
        pass

    def restrict_system_execution(self):
        self.restricted_events += [
            "os.system",
            "subprocess.Popen",
        ]

    def restrict_globals(self):
        pass

    def execute(self):
        sys.addaudithook(self.__hook)