# ===================
# Copyright (c) 2022 Tyler Pixley, all rights reserved.
# 
# This file (base_solar_cal.py) is part of TimelineBuilder.
#
# TimelineBuilder is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later version.
#
# TimelineBuilder is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# TimelineBuilder. If not, see <https://www.gnu.org/licenses/>.
# ===================

# ===================
# Implementing a simple solar calendar with 6-day weeks, 30-day months, and 10-month years.
# Day 0 corresponds to the date "Oneday, 1 Onemonth 1", where dates are formatted "D Month Y".
# Year 1 is directly preceded by Year -1, so there is no "Year 0".
# ===================
month_names = ["Onemonth", "Twomonth", "Threemonth", "Fourmonth", "Fivemonth",
                "Sixmonth", "Sevenmonth", "Eightmonth", "Ninemonth", "Tenmonth"]
weekdays = ["Oneday", "Twoday", "Threeday", "Fourday", "Fiveday", "Sixday"]

def init_calendar():
    return

def get_day_of_week(in_date: int) -> str:
    # Python modulo doesn't require compensating for negative numerators in this case.
    # Day -1 is a Sixday
    return weekdays[in_date % 6]

def get_broken_date_length() -> int:
    # Broken dates represent a day, month, and year
    return 3

def get_date_format() -> str:
    return "%2 %1 %0"

def get_timespan_format() -> str:
    return "%0 years %1 months %2 days"

def format_date(in_date: int) -> str:
    return format_broken_date_impl(in_date, break_date(in_date))

def format_broken_date(in_date: list[int]) -> str:
    return format_broken_date_impl(combine_date(in_date), in_date)

def format_broken_date_impl(in_date: int, in_broken_date: list[int]):
    if not validate_date(in_broken_date):
        raise RuntimeError("Invalid date!")

    date_len: int = len(in_broken_date)

    if date_len == get_broken_date_length():
        day_of_week: str = get_day_of_week(in_date)
        month: str = month_names[in_broken_date[1] - 1]
        return "{weekday}, {day} {month} {year}".format(weekday = day_of_week, day = in_broken_date[2], month = month, year = in_broken_date[0])
    elif date_len == 2:
        month: str = month_names[in_broken_date[1] - 1]
        return "{month} {year}".format(month = month, year = in_broken_date[0])
    else:
        return str(in_broken_date[0])

def format_date_span(start_date: int, end_date: int) -> str:
    return format_timespan(break_date_span(start_date, end_date))

def format_timespan(in_span: list[int]) -> str:
    span_len: int = len(in_span)
    if span_len == 0 or span_len > get_broken_date_length():
        raise RuntimeError("Invalid timespan!")

    has_years: bool = in_span[0] != 0
    has_months: bool = span_len >= 2 and in_span[1] != 0
    has_days: bool = span_len == 3 and in_span[2] != 0

    year_str: str = ""
    if has_years:
        if in_span[0] == 1:
            year_str = "1 year"
        else:
            year_str = "{num} years".format(num = in_span[0])

        if has_months or has_days:
            year_str = year_str + ", "
    
    month_str: str = ""
    if has_months:
        if in_span[1] == 1:
            month_str = "1 month"
        else:
            month_str = "{num} months".format(num = in_span[1])

        if has_days:
            month_str = month_str + ", "

    day_str: str = ""
    if has_days:
        if in_span[2] == 1:
            day_str = "1 day"
        else:
            day_str = "{num} days".format(num = in_span[2])

    return year_str + month_str + day_str

def break_date(in_date: int) -> list[int]:
    if in_date >= 0:
        return [in_date // 300 + 1, (in_date // 30) % 10 + 1, in_date % 30 + 1]
    else:
        return [in_date // 300, (in_date // 30) % 10 + 1, in_date % 30 + 1]

def break_date_span(start_date: int, end_date: int) -> list[int]:
    broken_start: list[int] = break_date(start_date)
    broken_end: list[int] = break_date(end_date)

    date_diff: list[int] = [end - start for start, end in zip(broken_start, broken_end)]

    # Move any extra (or negative) days to the month
    date_diff[1] = date_diff[1] + (date_diff[0] // 30)
    date_diff[2] = date_diff[2] % 30

    # Move any extra (or negative) months to the year
    date_diff[0] = date_diff[0] + (date_diff[1] // 10)
    date_diff[1] = date_diff[1] % 10

    return date_diff

def combine_date(in_date: list[int]) -> int:
    if not validate_date(in_date):
        raise RuntimeError("Invalid date!")

    date_len: int = len(in_date)

    year_days: int = (in_date[0] - 1) * 300

    month_days: int = 0
    if date_len >= 2:
        if in_date[0] > 0:
            month_days = (in_date[1] - 1) * 30
        else:
            month_days = (10 - in_date[1]) * 30

    days: int = 0
    if date_len == get_broken_date_length():
        if in_date[0] > 0:
            days = in_date[2] - 1
        else:
            days = 30 - in_date[2]

    if in_date[0] > 0:
        return year_days + month_days + days
    else:
        return -1 * (abs(year_days) + month_days + days) - 1

def move_date(start_date: int, delta_span: list[int]) -> int:
    delta_len: int = len(delta_span)

    if delta_len == 0 or delta_len > get_broken_date_length():
        raise RuntimeError("Invalid delta span!")
    
    year_offset: int = delta_span[0] * 300

    month_offset: int = 0
    if delta_len >= 2:
        month_offset: int = delta_span[1] * 30

    day_offset: int = 0
    if delta_len == get_broken_date_length():
        day_offset = delta_span[2]

    return start_date + year_offset + month_offset + day_offset

def validate_date(in_date: list[int]) -> bool:
    date_len = len(in_date)

    valid_len: bool = date_len > 0 and date_len <= get_broken_date_length()
    if not valid_len:
        return False

    valid_days: bool = date_len < 3 or (in_date[2] > 0 and in_date[2] <= 30)
    valid_months: bool = date_len < 2 or (in_date[1] > 0 and in_date[1] <= 10)
    valid_years: bool = in_date[0] != 0

    return valid_days and valid_months and valid_years