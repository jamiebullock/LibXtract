const xtract = require('./xtract');

console.log('\nRunning libxtract Node.js bindings test...\n');

const len = 8;

const a = new xtract.doubleArray(len);
const temp = [];

for (let i = 0; i < len; i++) {
    a.setitem(i, 2.0 * i);
    temp.push(2.0 * i);
}

const mean = xtract.xtract_mean(a.cast(), len, null)[1];
console.log('The mean of ' + temp.join(', ') + ' is: ' + mean.toFixed(2));

const argv = new xtract.doubleArray(1);
argv.setitem(0, mean);

const variance = xtract.xtract_variance(a.cast(), len, xtract.doublea_to_voidp(argv.cast()))[1];
console.log('The variance is ' + variance.toFixed(2));

console.log('Computing spectrum...');

const specArgv = new xtract.doubleArray(4);
specArgv.setitem(0, 44100.0 / len);
specArgv.setitem(1, xtract.XTRACT_MAGNITUDE_SPECTRUM);
specArgv.setitem(2, 0.0);
specArgv.setitem(3, 0.0);

xtract.xtract_init_fft(len, xtract.XTRACT_SPECTRUM);

const result = new xtract.doubleArray(len);
xtract.xtract_spectrum(a.cast(), len, xtract.doublea_to_voidp(specArgv.cast()), result.cast());

for (let i = 0; i < len; i++) {
    console.log(result.getitem(i));
}

console.log('Testing stateful functions...');

const N = 4;
const state = xtract.xtract_last_n_state_new(N);
const lastN = new xtract.doubleArray(N);
for (let i = 0; i < N; i++) lastN.setitem(i, 0.0);

const data = new xtract.doubleArray(1);
data.setitem(0, 42.0);
// N.B. data is a single sample; N is the window size, not the size of data
xtract.xtract_last_n(state, data.cast(), N, null, lastN.cast());
console.log('last_n after one value: ' + lastN.getitem(N - 1).toFixed(1));

data.setitem(0, 99.0);
xtract.xtract_last_n(state, data.cast(), N, null, lastN.cast());
console.log('last_n after two values: ' + lastN.getitem(N - 2).toFixed(1) + ', ' + lastN.getitem(N - 1).toFixed(1));

xtract.xtract_last_n_state_delete(state);

console.log('\nFinished!\n');
