Object.defineProperty(new Function('return this')(), 'window', { value: new Function('return this')(), writable: false, enumerable: true, configurable: false});

M = {
    compare: function(val, targ) {
            return Duktape.enc('jc', targ) == Duktape.enc('jc', val);
        },
    copy: function(x) {
        return Duktape.dec('jc', Duktape.enc('jc', x));
    }
}

var logger = new Duktape.Logger(null);

___scopes = {
    input: {
        changes: {},
        check: function(targ, val) {
            return Duktape.enc('jc', targ) !== Duktape.enc('jc', val);
        }
    },
    output: {
        changes: {},
        check: function(targ, val) {
            return Duktape.enc('jc', targ) !== Duktape.enc('jc', val);
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

___reg = {};

function copy(x) {
    return Duktape.dec('jc', Duktape.enc('jc', x));
}

function log_window() {
    var Z = {};
    for(var k in window) {
        if(k !== "window" && k !== "Z" && typeof window[k] !== "function") {
            Z[k] = window[k];
        }
    }
    return Duktape.enc('jc', Z);
}

function get_var(name) {
    if(name.search('.') === -1) {
        return window[name];
    }
    else {
        var v = window;
        var splitted = name.split('.');
        for(var _k in splitted) {
            var k = splitted[_k];
            if(!(k in v)) {
                v[k] = {};
            }
            v = v[k];
        }
        return v;
    }
}

log_set_var = {}

function set_var(name, val) {
    var v = window;
    log_set_var[name] = name.split('.');
    var splitted = name.split('.');
    var _k;
    for(_k in splitted) {
        var k = splitted[_k];
        if(!(k in v) && _k !== splitted.length - 1)
            v[k] = {};
        else if(_k !== splitted.length -1)
            v = v[k];
    }
    v[k] = val;
}


function add(sc, init, name) {
    set_var(name, init);
    window.___reg[name] = {
        old: copy(init),
        scope: window.___scopes[sc],
        var: name
    };

}

function poll_changes() {
    for(var a in window.___reg) {
        var v = window.___reg[a];
        if(v.scope.check(v.old, get_var(v.var))) {
            v.scope.changes[v.var] = true;
        }
    }
}



function get_changes(scope) {
    var changes = {};
    for(var v in window.___scopes[scope].changes) {
        changes[v] = Duktape.enc('jc', get_var(v));
    }
    return Duktape.enc('jc', changes);
}

function get_number_of_changes(scope) {
    var n = 0;
    for(var v in window.___scopes[scope].changes) {
        n++;
    }
    return n;
}


function apply_changes(scope) {
    if(scope === undefined) {
        for(var s in ___scopes)
            apply_changes(s);
    }
    else {
        for(var v in window.___scopes[scope].changes) {
            window.___reg[v].old = copy(get_var(v));
        }
    }
}

function restore_changes(scope) {
    if(scope === undefined) {
        for(var s in ___scopes)
            restore_changes(s);
    }
    else {
        for (var v in window.___scopes[scope].changes) {
            set_var(v, copy(window.___reg[v].old));
        }
        clear_changes(scope);
    }
}

function clear_changes(scope) {
    if(scope === undefined)
        for(var sc in window.___scopes) window.___scopes[sc].changes = {}
    else
        window.___scopes[scope].changes = {}
}
