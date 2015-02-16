
## Usage

### Build

```bash
make -C src asm2graphs
```

### Test

```bash
make -C tests check
```

### Generate JSON for one ASM file

```bash
$MS_ASM_HOME/scripts/generate.sh $MS_ASM_HOME/tests/test.asm
```

This command will create the directory *test* and the archive *test.tar.gz* in the current directory.
The directory *test* will contain one Call Graph stored in *test.json*.
For each procedure, a file *test-_proc\_label_.json* is created, it contains the Control Flow Graph (CFG) of the corresponding procedures.
*proc\_label* is either starting by *sub\_* if identified by IDA-PRO, or *rtn\_* if identfied by our analysis.
Procedures are made of blocks. Block's labels are starting by *loc\_* if identified by IDA-PRO, or *label_* if identfied by our analysis.
Call Graph (*test.json*):
```json
{
  "routines": [
    {
      "label":"rtn_0",
      "callees":[ { "label":"sub_0" } ]
    },{
      "label":"sub_0",
      "callees":[ { "label":"sub_1" } ]
    },{
      "label":"rtn_1",
      "callees":[]
    },{
      "label":"sub_1",
      "callees":[ { "label":"sub_2" } ]
    },{
      "label":"rtn_2",
      "callees":[]
    },{
      "label":"sub_2",
      "callees":[]
    }
  ]
}
```
Control Flow Graph for *sub\_0* (*test-sub\_0.json*): 
```json
{
  "blocks":[
    {
      "label":"sub_0",
      "out_true":"loc_1",
      "out_false":"",
      "last_inst":"",
      "instructions":[ {"str":"mov eax, ebx"} ],
      "callees":[]
    },{
      "label":"loc_1",
      "out_true":"loc_2",
      "out_false":"label_2",
      "last_inst":"jz",
      "instructions":[ {"str":"test eax"},{"str":"jz loc_2"} ],
      "callees":[]
    },{
      "label":"label_2",
      "out_true":"loc_1",
      "out_false":"",
      "last_inst":"jmp",
      "instructions":[ {"str":"call sub_1"},{"str":"jmp loc_1"} ],
      "callees":[ {"label":"sub_1"} ]
    },{
      "label":"loc_2",
      "out_true":"",
      "out_false":"",
      "last_inst":"retn",
      "instructions":[ {"str":"mov eax, ebx"},{"str":"retn"} ],
      "callees":[]
    }
  ]
}
```


### Batch processing

Given a file ("malwares.lst") which contains a list of malware with absolute path:
```bash
for malware in `cat malwares.lst`
do
  $MS_ASM_HOME/scripts/generate.sh $malware
  rm -rf ${malware%.*} # to save space
done
tar czf malwares.tar.gz *.tar.gz 
```

