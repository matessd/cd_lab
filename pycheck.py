import json
from os import system
from itertools import count

def msg(s):
    print('\033[1m\033[91m' + s + '\033[0m\033[0m');
def err(data_in, s):
    msg("Wrong on input " + str(data_in) + "\n" + s)
    exit(1)

f_ir   = "./workdir/a.ir";
f_json = "./workdir/a.json";
program = "./irsim.py"
irsim_in  = './workdir/irsim_in'
irsim_out = './workdir/irsim_out'
irsim_err = './workdir/irsim_err'

for data_in, data_out, ret_val in json.load(open(f_json)):
    with open(irsim_in, 'w') as to_irsim_w:
        for i in data_in:
            to_irsim_w.write(str(i) + '\n')

    ret = system("python %s %s < %s 2>%s"%(program, f_ir , irsim_in, irsim_err))
    #ret = system("python %s %s < %s > %s 2>%s"%(program, f_ir , irsim_in , irsim_out, irsim_err))
    # Suppose irsim is compiled by run.sh
    if ret != 0:
        with open(irsim_err, 'r') as from_irsim_err:
            err(data_in,
                "runtime error occured when running your IR code\n"
                + from_irsim_err.read().splitlines()[-1]);
    with open(irsim_out, 'r') as from_irsim_r:
        my_out = from_irsim_r.readlines()
        # Filter out the first line "load ./workdir/a.ir"
        if len(data_out) != len(my_out):
            err(data_in, "Output mismatch!(output less or more)")
        try:
            for idx, expect, user_out in zip(count(1), data_out, my_out):
                user_out = int(user_out)
                if expect != user_out:
                    err(data_in, "Output mismatch! expected %d, found %d at line %d" % (expect, user_out, idx));
            else:
                #if "" != from_irsim_r.readline():
                    #err(data_in, "Output mismatch!(you output more than supposed?)")
                with open("./workdir/irsim_err", "r+") as from_irsim_err, open("./workdir/count", "r+") as f:    
                    from_irsim_err.readline()
                    # Filter out the first line "load ./workdir/a.ir"
                    cnt = int(from_irsim_err.readline()) + int(f.read())
                    f.seek(0)
                    f.write(str(cnt))
        except ValueError:
            err(data_in, "Output mismatch!(you output less than supposed?)")

exit(0)
