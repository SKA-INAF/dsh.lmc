import re
import unicodedata
import textwrap


def _string_sanity_check(string):
    if string is None:
        return ''
    if not isinstance(string, basestring):
        return str(string)
    return string


''' Converts underscored or dasherized string to a camelized one. Begins with a lower case letter unless it starts with an underscore, dash or an upper case letter. '''


def camelize(string, uppercase_first_letter=True):
    sanitzed_string = _string_sanity_check(string)
    replaced = re.sub(r"^[-_\s]+", "", sanitzed_string)
    if uppercase_first_letter:
        return re.sub(r"(?:^|[-_\s]+)(.)", lambda m: m.group(1).upper(), replaced)
    else:
        return replaced[0].lower() + camelize(replaced)[1:]

''' Converts string to camelized class name. First letter is always upper case. '''


def classify(string):
    sanitzed_string = _string_sanity_check(string)
    replaced = re.sub(
        r"(?:^|[\.]+)(.)", lambda m: m.group(1).upper(), sanitzed_string)
    return camelize(replaced)

''' Trim and replace multiple spaces with a single space. '''


def clean(string):
    sanitzed_string = _string_sanity_check(string)
    return " ".join(sanitzed_string.split())

''' Counts the number of times needle is in haystack '''


def count(haystack, needle):
    haystack = _string_sanity_check(haystack)
    needle = _string_sanity_check(needle)
    if needle is '' or needle is None:
        return 0
    return haystack.count(needle)

''' Converts a underscored or camelized string into an dasherized one '''


def dasherize(string):
    sanitzed_string = _string_sanity_check(string)
    striped = sanitzed_string.strip()
    replaced = re.sub(r'([A-Z])', r'-\1', striped)
    replaced = re.sub(r'[-_\s]+', r'-', replaced)
    replaced = re.sub(r'^-', r'', replaced)
    return replaced.lower()

''' Converts first letter of the string to lowercase. '''


def decapitalize(string):
    sanitzed_string = _string_sanity_check(string)
    if sanitzed_string is '' or sanitzed_string is None:
        return ''
    return sanitzed_string[0].lower() + sanitzed_string[1:]


''' Dedent unnecessary indentation. '''


def dedent(string):
    sanitzed_string = _string_sanity_check(string)
    if sanitzed_string is '' or sanitzed_string is None:
        return ''
    return textwrap.dedent(sanitzed_string)

''' Checks whether the string ends with needle at position (default: haystack.length). '''


def ends_with(haystack, needle, beg=0, end=None):
    haystack = _string_sanity_check(haystack)
    needle = _string_sanity_check(needle)
    if end is None:
        end = len(haystack)
    return haystack.endswith(needle, beg, end)


''' Converts an underscored, camelized, or dasherized string into a humanized one. Also removes beginning and ending whitespace, and removes the postfix '_id'. '''


def humanize(string):
    if string is '' or string is None:
        return ''
    sanitzed_string = _string_sanity_check(string)
    underscored = underscore(sanitzed_string)
    replaced = re.sub('_id$', '', underscored)
    replaced = replaced.replace("_", ' ')
    replaced = replaced.replace("-", ' ')
    striped = replaced.strip()
    firstUp = striped[0].upper() + striped[1:]
    return re.sub(' +', ' ', firstUp)

''' Tests if string contains a substring. '''


def includes(haystack, needle):
    haystack = _string_sanity_check(haystack)
    needle = _string_sanity_check(needle)
    if needle in haystack:
        return True
    else:
        return False

''' Insert word in string at the defined position '''


def insert(string, index, substring):
    sanitzed_string = _string_sanity_check(string)
    sanitzed_substring = _string_sanity_check(substring)
    return sanitzed_string[:index] + sanitzed_substring + sanitzed_string[index:]

''' Returns split lines as an array '''


def lines(string):
    sanitzed_string = _string_sanity_check(string)
    return re.split('\r\n?|\n', sanitzed_string)

''' Return the string left justified in a string of length width. '''


def lpad(string, width, fillchar=' '):
    sanitzed_string = _string_sanity_check(string)
    return sanitzed_string.rjust(width, fillchar)

''' Return a copy of the string with leading characters removed. '''


def ltrim(string, chars=None):
    sanitzed_string = _string_sanity_check(string)
    if isinstance(chars, int):
        chars = str(chars)
    return sanitzed_string.lstrip(chars)

''' Repeats a string count times. '''


def repeat(string, count, separator=None):
    sanitzed_string = _string_sanity_check(string)
    sanitzed_count = int(count)
    if separator is None:
        return sanitzed_count * sanitzed_string
    sanitzed_separator = str(separator)
    return (sanitzed_count * (sanitzed_string + str(sanitzed_separator)))[:-len(sanitzed_separator)]

''' Return the string right justified in a string of length width. '''


def rpad(string, width, fillchar=' '):
    sanitzed_string = _string_sanity_check(string)
    return sanitzed_string.ljust(width, fillchar)

''' Return a copy of the string with trailing characters removed. '''


def rtrim(string, chars=None):
    sanitzed_string = _string_sanity_check(string)
    if isinstance(chars, int):
        chars = str(chars)
    return sanitzed_string.rstrip(chars)

''' Replace string in string '''


def splice(string, index, how_many, substring):
    sanitzed_string = _string_sanity_check(string)
    sanitzed_substring = _string_sanity_check(substring)
    return sanitzed_string[:index] + sanitzed_substring + sanitzed_string[index + how_many:]

''' Checks whether the string begins with the needle at position (default: 0). '''


def starts_with(haystack, needle, beg=0, end=None):
    sanitzed_haystack = _string_sanity_check(haystack)
    sanitzed_needle = _string_sanity_check(needle)
    if end is None:
        end = len(sanitzed_haystack)
    return sanitzed_haystack.startswith(sanitzed_needle, beg, end)

''' Returns the successor to string '''


def successor(string):
    sanitzed_string = _string_sanity_check(string)
    strip_zs = sanitzed_string.rstrip('z')
    if strip_zs:
        return strip_zs[:-1] + chr(ord(strip_zs[-1]) + 1) + 'a' * (len(sanitzed_string) - len(strip_zs))
    else:
        return 'a' * (len(sanitzed_string) + 1)

''' Returns a copy of the string in which all the case-based characters have had their case swapped.'''


def swap_case(string):
    sanitzed_string = _string_sanity_check(string)
    return string.swapcase()


def transliterate(string):
    sanitzed_string = _string_sanity_check(string)
    normalized = unicodedata.normalize('NFKD', sanitzed_string)
    return normalized.encode('ascii', 'ignore').decode('ascii')

''' Converts a underscored or camelized string into an dasherized one '''


def underscore(string):
    sanitzed_string = _string_sanity_check(string)
    replaced = re.sub(r"([A-Z]+)([A-Z][a-z])", r'\1_\2', sanitzed_string)
    replaced = re.sub(r"([a-z\d])([A-Z])", r'\1_\2', replaced)
    return replaced.replace("-", "_").lower()


# ---


class FilterModule(object):

    def filters(self):
        return {
            'camelize': camelize,
            'clean': clean,
            'classify': classify,
            'count': count,
            'dasherize': dasherize,
            'decapitalize': decapitalize,
            'dedent': dedent,
            'ends_with': ends_with,
            'humanize': humanize,
            'includes': includes,
            'insert': insert,
            'lines': lines,
            'lpad': lpad,
            'ltrim': ltrim,
            'rpad': rpad,
            'rtrim': rtrim,
            'repeat': repeat,
            'splice': splice,
            'starts_with': starts_with,
            'successor': successor,
            'swap_case': swap_case,
            'transliterate': transliterate,
            'underscore': underscore
        }
