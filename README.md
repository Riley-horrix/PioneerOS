# Running

`make qemu-debug`

Then in a separate terminal run 

`make lldb`

Inside lldb you will need to run 

```
(lldb) gdb-remote 1234
```

To exit from qemu type :

`crtl + a` then `x`