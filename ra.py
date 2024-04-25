class Symbol:
    def __init__(self, name, register=None, location=-1):
        self.name = name
        self.register = register
        self.location = location

class Register:
    def __init__(self, name):
        self.name = name

    def __str__(self):
        return self.name

class RegisterPool:
    def __init__(self):
        self.registers = [Register("EDI"), Register("ESI")]
        self.available = self.registers.copy()

    def get_register(self):
        if not self.available:
            raise ValueError("No remaining register")
        return self.available.pop(0)

    def free_register(self, register):
        self.available.append(register)

    def is_empty(self):
        return not self.available

class LiveInterval:
    def __init__(self, symbol, start_point, end_point):
        self.symbol = symbol
        self.start_point = start_point
        self.end_point = end_point

def delete_interval(intervals, index):
    return intervals[:index] + intervals[index+1:]

def expire_old_intervals(active_intervals, current_interval, pool):
    active_intervals.sort(key=lambda x: x.end_point)
    for i, interval in enumerate(active_intervals):
        if interval.end_point >= current_interval.start_point:
            return active_intervals
        pool.free_register(interval.symbol.register)
        return delete_interval(active_intervals, i)
    return active_intervals

def spill_at_interval(active_intervals, interval, current_stack_location):
    active_intervals.sort(key=lambda x: x.end_point)
    spill = active_intervals[-1]
    if spill.end_point > interval.end_point:
        print(f"ACTION: SPILL INTERVAL {spill.symbol.name} and ALLOCATE REGISTER {spill.symbol.register} TO {interval.symbol.name}")
        interval.symbol.register = spill.symbol.register
        spill.symbol.location = current_stack_location
        active_intervals = delete_interval(active_intervals, len(active_intervals)-1)
    else:
        print(f"ACTION: SPILL INTERVAL {interval.symbol.name}")
        interval.symbol.location = current_stack_location
    current_stack_location += 4
    return current_stack_location

pool = RegisterPool()
current_stack_location = 0
live_intervals = [
    LiveInterval(Symbol("b"), 1, 5),
    LiveInterval(Symbol("this"), 2, 9),
    LiveInterval(Symbol("tdv"), 3, 10),
    LiveInterval(Symbol("983"), 4, 7),
    LiveInterval(Symbol("this"), 2, 9),
    LiveInterval(Symbol("984"), 5, 7),
    LiveInterval(Symbol("985"), 7, 12),
    LiveInterval(Symbol("986"), 8, 12),
    LiveInterval(Symbol("987"), 9, 11),
    LiveInterval(Symbol("988"), 10, 12),
    

]
active_intervals = []

live_intervals.sort(key=lambda x: x.start_point)

for interval in live_intervals:
    active_intervals = expire_old_intervals(active_intervals, interval, pool)

    if pool.is_empty():
        current_stack_location = spill_at_interval(active_intervals, interval, current_stack_location)
    else:
        register = pool.get_register()
        print(f"ACTION: ALLOCATE REGISTER {register} TO INTERVAL {interval.symbol.name}")
        interval.symbol.register = register
        active_intervals.append(interval)

for interval in live_intervals:
    print(f"SYMBOL: {interval.symbol.name} | LOCATION: {interval.symbol.location} | REGISTER: {interval.symbol.register}")