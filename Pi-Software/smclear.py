def padString(string):
    for x in range(0, 4096 - len(string)):
        string += ' '

    return string
