static tcl_result_t tcl_cmd_math(struct tcl *tcl, tcl_value_t *args, void *arg) {
    (void)arg;
    char buf[64];
    tcl_value_t *opval = tcl_list_at(args, 0);
    tcl_value_t *aval = tcl_list_at(args, 1);
    tcl_value_t *bval = tcl_list_at(args, 2);
    const char *op = tcl_string(opval);
    int opnum = op[0] << 8 | op[1];
    float a = tcl_num(aval);
    float b = tcl_num(bval);
    float c = 0;
    switch (opnum) {
        case 0x2b00: c = a + b; break;
        case 0x2d00: c = a - b; break;
        case 0x2a00: c = a * b; break;
        case 0x2f00: c = a / b; break;
        case 0x3e00: c = a > b; break;
        case 0x3e3d: c = a >= b; break;
        case 0x3c00: c = a < b; break;
        case 0x3c3d: c = a <= b; break;
        case 0x3d3d: c = a == b; break;
        case 0x213d: c = a != b; break;
    }
    sprintf(buf, "%f", c);
    tcl_free(opval);
    tcl_free(aval);
    tcl_free(bval);
    return tcl_result(tcl, TCL_OK, tcl_alloc(buf, strlen(buf)));
}

void tcl_init_math(struct tcl *tcl) {
    char *math[] = {"+", "-", "*", "/", ">", ">=", "<", "<=", "==", "!="};
    for (unsigned int i = 0; i < (sizeof(math) / sizeof(math[0])); i++) {
        tcl_register(tcl, math[i], tcl_cmd_math, 3, NULL);
    }
}