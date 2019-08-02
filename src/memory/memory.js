Object.defineProperty(new Function('return this')(), 'window', {
    value: new Function('return this')(),
    writable: false, enumerable: true, configurable: false
});


___scopes = {
    input: {
        changes: {},
        check: function(targ, val) {
            return JSON.stringify(targ) !== JSON.stringify(val);
        }
    },
    output: {
        changes: {},
        check: function(targ, val) {
            return JSON.stringify(targ) !== JSON.stringify(val);
        }
    },
    output_no_zero: {
        changes: {},
        check: function(targ, val) {
            return targ != val && val !== 0;
        }
    },
    pure_output: {
        changes: {},
        check: function(targ,  val) {
            return true;
        }
    }
};

___reg = {}

function copy(x) {
    return JSON.parse(JSON.stringify(x));
}

function add(sc, init, name) {
    window[name] = init;
    window.___reg[name] = {
        old: copy(init),
        scope: window.___scopes[sc],
        var: name
    };
}

function poll_changes() {
    for(let a in ___reg) {
        let v = ___reg[a];
        if(v.scope.check(v.old, window[v.var])) {
            v.scope.changes[v.var] = true;
        }
    }
}

function get_changes(scope) {
    var changes = {};
    for(let v in window.___scopes[scope].changes) {
        changes[v] = JSON.stringify(window[v]);
    }
    return JSON.stringify(changes);
}

function get_number_of_changes(scope) {
    var n = 0;
    for(let v in window.___scopes[scope].changes) {
        n++;
    }
    return n;
}

function apply_changes(scope) {
    if(scope === undefined) {
        for(let s in ___scopes)
            restore_changes(s);
    }
    else {
        for(let v in window.___scopes[scope].changes) {
            window.___reg[v].old = copy(window[v]);
        }
    }
}

function restore_changes(scope) {
    if(scope === undefined) {
        for(let s in ___scopes)
            restore_changes(s);
    }
    else {
        for (let v in window.___scopes[scope].changes) {
            window[v] = copy(window.___reg[v].old);
        }
        clear_changes(scope);
    }
}

function clear_changes(scope) {
    if(scope === undefined)
        for(let sc in window.___scopes) window.___scopes[sc].changes = {}
    else
        window.___scopes[scope].changes = {}
}
