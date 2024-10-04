class SA:
    id = 0
    s: str = None
    idx = 1
    cur_char: chr = None


    def __init__(self, string: str):
        self.s = string
        self.cur_char = self.s[0]


    def get_next_char(self):
        self.s += "$"
        self.idx += 1
        return self.s[self.idx - 1]


    def preambula(self, from_id, name):
        local_id = self.id
        print(f'{local_id} [label = "{name}"];')
        self.id += 1
        print(f"{from_id} -> {local_id};")

        return local_id


    def B1(self, from_id):
        self.preambula(from_id, "B1")

        if self.cur_char in ["b", "a"]:
            self.cur_char = self.get_next_char()
        else:
            raise ValueError


    def B2(self, from_id):
        local_id = self.preambula(from_id, "B2")

        if self.cur_char in ["c", "$"]:
            pass
        elif self.cur_char in ["b", "a"]:
            self.B1(local_id)
            self.B2(local_id)
        else:
            raise ValueError


    def B(self, from_id):
        local_id = self.preambula(from_id, "B")

        if self.cur_char != "b":
            raise ValueError

        self.cur_char = self.get_next_char()
        self.B2(local_id)


    def A(self, from_id):
        local_id = self.preambula(from_id, "A")

        if self.cur_char == "c":
            self.cur_char = self.get_next_char()
        elif self.cur_char == "a":
            self.cur_char = self.get_next_char()
            self.A(local_id)
            if self.cur_char != "a":
                raise ValueError
            self.cur_char = self.get_next_char()
        else:
            raise ValueError


    def C3(self, from_id):
        local_id = self.preambula(from_id, "C3")

        if self.cur_char == "c":
            self.cur_char = self.get_next_char()
            self.C3(local_id)
        elif self.cur_char == "b":
            self.B(local_id)
        elif self.cur_char == "a":
            self.cur_char = self.get_next_char()
            self.A(local_id)
            if self.cur_char != "a":
                raise ValueError
            self.cur_char = self.get_next_char()
        elif self.cur_char == "$":
            pass
        else:
            raise ValueError


    def C2(self, from_id):
        local_id = self.preambula(from_id, "C2")

        if self.cur_char == "c":
            self.cur_char = self.get_next_char()
            self.C3(local_id)
        elif self.cur_char == "b":
            self.B(local_id)
        elif self.cur_char == "$":
            pass
        else:
            raise ValueError


    def C1(self, from_id):
        local_id = self.preambula(from_id, "C1")

        if self.cur_char == "c":
            self.cur_char = self.get_next_char()
            self.C2(local_id)
        elif self.cur_char == "$":
            pass
        else:
            raise ValueError


    def C(self, from_id):
        local_id = self.preambula(from_id, "C")

        if self.cur_char == "c":
            self.cur_char = self.get_next_char()
            self.C1(local_id)
        else:
            raise ValueError


    def S(self):
        local_id = self.id
        print(f'{local_id} [label = "S"];')
        self.id += 1
        self.A(local_id)
        self.B(local_id)
        self.C(local_id)

if __name__ == "__main__":
    sa = SA(input())
    print("digraph {")
    sa.S()
    print("}")

