static tcl_result_t tcl_cmd_math(struct tcl *tcl, tcl_string_t *args, void *arg) {
    (void)arg;
    char buf[64];
    tcl_string_t *opval = tcl_list_at(args, 0);
    tcl_string_t *aval = tcl_list_at(args, 1);
    tcl_string_t *bval = tcl_list_at(args, 2);
    const char *op = tcl_string(opval);
    int a = tcl_int(aval);
    int b = tcl_int(bval);
    int c = 0;
    if (op[0] == '+') {
        c = a + b;
    } else if (op[0] == '-') {
        c = a - b;
    } else if (op[0] == '*') {
        c = a * b;
    } else if (op[0] == '/') {
        c = a / b;
    } else if (op[0] == '>' && op[1] == '\0') {
        c = a > b;
    } else if (op[0] == '>' && op[1] == '=') {
        c = a >= b;
    } else if (op[0] == '<' && op[1] == '\0') {
        c = a < b;
    } else if (op[0] == '<' && op[1] == '=') {
        c = a <= b;
    } else if (op[0] == '=' && op[1] == '=') {
        c = a == b;
    } else if (op[0] == '!' && op[1] == '=') {
        c = a != b;
    }
    sprintf(buf, "%d", c);
    tcl_free(opval);
    tcl_free(aval);
    tcl_free(bval);
    return tcl_result(tcl, TCL_OK, tcl_alloc(buf, strlen(buf)));
}

void tcl_init_math(struct tcl *tcl) {
    char *math[] = {"+", "-", "*", "/", ">", ">=", "<", "<=", "==", "!="};
    for (unsigned int i = 0; i < (sizeof(math) / sizeof(math[0])); i++) {
        tcl_register(tcl, math[i], tcl_cmd_math, 0x22, NULL);
    }
}