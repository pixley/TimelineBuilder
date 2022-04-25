class BaseSolarCal:
    def __init__(self):
        self.month_names = ["Onemonth", "Twomonth", "Threemonth", "Fourmonth", "Fivemonth",
                            "Sixmonth", "Sevenmonth", "Eightmonth", "Ninemonth", "Tenmonth"]
        self.weekdays = ["Oneday", "Twoday", "Threeday", "Fourday", "Fiveday", "Sixday"]

    def get_broken_date_length(self) -> int:
        return 3

    def get_date_format(self) -> str:
        return "%2 %1 %0"

    def get_timespan_format(self) -> str:
        return "%0 years %1 months %2 days"

    def format_date(self, in_date: int) -> str:
        return "Oneday, 1 Onemonth 1"

    def format_broken_date(self, in_date: list[int]) -> str:
        return "Oneday, 1 Onemonth 1"

    def format_date_span(self, start_date: int, end_date: int) -> str:
        return "1 day"

    def format_timespan(self, in_span: list[int]) -> str:
        return "1 day"

    def break_date(self, in_date: int) -> list[int]:
        return [1, 1, 1]

    def break_date_span(self, start_date: int, end_date: int) -> list[int]:
        return [1, 0, 0]

    def combine_date(self, in_date: list[int]) -> int:
        return 1

    def move_date(self, start_date: int, delta_span: list[int]) -> int:
        return 1

    def validate_date(self, in_date: list[int]) -> bool:
        return in_date.count() == 3 and in_date[0] > 0 and in_date[0] <= 30 and in_date[1] > 0 and in_date[1] <= 10 and in_date[2] != 0

active_calendar = None

def init_calendar():
    global active_calendar
    active_calendar = BaseSolarCal()

def get_broken_date_length() -> int:
    return active_calendar.get_broken_date_length()

def get_date_format() -> str:
    return active_calendar.get_date_format()

def get_timespan_format() -> str:
    return active_calendar.get_timespan_format()

def format_date(in_date: int) -> str:
    return active_calendar.format_date(in_date)

def format_broken_date(in_date: list[int]) -> str:
    return active_calendar.format_broken_date(in_date)

def format_date_span(start_date: int, end_date: int) -> str:
    return active_calendar.format_date_span(start_date, end_date)

def format_timespan(in_span: list[int]) -> str:
    return active_calendar.format_timespan(in_span)

def break_date(in_date: int) -> list[int]:
    return active_calendar.break_date(in_date)

def break_date_span(start_date: int, end_date: int) -> list[int]:
    return active_calendar.break_date_span(start_date, end_date)

def combine_date(in_date: list[int]) -> int:
    return active_calendar.combine_date(in_date)

def move_date(start_date: int, delta_span: list[int]) -> int:
    return active_calendar.move_date(start_date, delta_span)

def validate_date(in_date: list[int]) -> bool:
    return active_calendar.validate_date(in_date)