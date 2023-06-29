import random


def random_bracket_sequence(n):
    """Generates a balanced sequence of n +1s and n -1s corresponding to correctly nested brackets."""
    # "Generating binary trees at random", Atkinson & Sack, 1992

    # Generate a randomly shuffled sequence of n +1s and n -1s
    # These are steps 1 and 2 of the algorithm in the paper
    seq = [-1, 1]*n
    random.shuffle(seq)

    # This now corresponds to a balanced bracket sequence (same number of
    # opening and closing brackets), but it might not be well-formed
    # (brackets closed before they open). Fix this up using the bijective
    # map in the paper (step 3).
    prefix = []
    suffix = []
    word = []
    partial_sum = 0
    for s in seq:
        word.append(s)
        partial_sum += s
        if partial_sum == 0:  # at the end of an irreducible balanced word
            if s == -1:  # it was well-formed! append it.
                prefix += word
            else:
                # it was not well-formed! fix it.
                prefix.append(1)
                suffix = [-1] + [-x for x in word[1:-1]] + suffix
            word = []

    return prefix + suffix


def is_valid_bracket_seq(seq):
    partial_sum = 0
    for x in seq:
        if x != -1 and x != 1:
            return False
        partial_sum += x
        if partial_sum < 0:
            return False
    return partial_sum == 0


n = 40
random.seed()
for i in range(1):
    res = random_bracket_sequence(n)
    assert len(res) == 2*n
    assert is_valid_bracket_seq(res)

    def f(x):
        return '(' if x == +1 else ')'

    s = [f(x) for x in res]
    s = ''.join(s)
    print(s)
