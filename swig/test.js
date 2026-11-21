/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

const xtract = require('./xtract');

console.log('\nRunning libxtract Node.js bindings test...\n');

const len = 8;

const a = new xtract.doubleArray(len);
const temp = [];

for (let i = 0; i < len; i++) {
    a.setitem(i, 2.0 * i);
    temp.push(2.0 * i);
}

const mean = xtract.xtract_mean(null, a.cast(), len, null)[1];
console.log('The mean of ' + temp.join(', ') + ' is: ' + mean.toFixed(2));

const argv = new xtract.doubleArray(1);
argv.setitem(0, mean);

const variance = xtract.xtract_variance(null, a.cast(), len, xtract.doublea_to_voidp(argv.cast()))[1];
console.log('The variance is ' + variance.toFixed(2));

console.log('Computing spectrum...');

const specArgv = new xtract.doubleArray(3);
specArgv.setitem(0, 44100.0 / len);
specArgv.setitem(1, xtract.XTRACT_MAGNITUDE_SPECTRUM);
specArgv.setitem(2, 0.0);

const ctx = xtract.xtract_context_new(len);

const result = new xtract.doubleArray(len);
xtract.xtract_spectrum(ctx, a.cast(), len, xtract.doublea_to_voidp(specArgv.cast()), result.cast());
xtract.xtract_context_delete(ctx);

for (let i = 0; i < len; i++) {
    console.log(result.getitem(i));
}

console.log('Computing the log energy a filter passes...');

const gains = new xtract.doubleArray(4);
const coefficients = new xtract.doubleArray(4);
[0.5, 0.25, 2.0, 1.0].forEach((gain, i) => gains.setitem(i, gain));
[1.0, 2.0, 3.0, 4.0].forEach((coefficient, i) => coefficients.setitem(i, coefficient));

const filterbank = xtract.xtract_init_filterbank(8, 1, gains.cast());
const energy = new xtract.doubleArray(1);
xtract.xtract_mel_spectrogram(null, coefficients.cast(), 4, xtract.filterbank_to_voidp(filterbank), energy.cast());
xtract.xtract_free_filterbank(filterbank);

console.log('The log energy is ' + energy.getitem(0).toFixed(4));

if (!(Math.abs(energy.getitem(0) - Math.log(11.0)) <= 1e-12)) {
    process.exit(1);
}

console.log('Testing stateful functions...');

const N = 4;
const state = xtract.xtract_last_n_state_new(N);
const lastN = new xtract.doubleArray(N);
for (let i = 0; i < N; i++) lastN.setitem(i, 0.0);

const data = new xtract.doubleArray(1);
data.setitem(0, 42.0);
// N.B. data is a single sample; N is the window size, not the size of data
xtract.xtract_last_n(null, data.cast(), N, xtract.last_n_state_to_voidp(state), lastN.cast());
console.log('last_n after one value: ' + lastN.getitem(N - 1).toFixed(1));

data.setitem(0, 99.0);
xtract.xtract_last_n(null, data.cast(), N, xtract.last_n_state_to_voidp(state), lastN.cast());
console.log('last_n after two values: ' + lastN.getitem(N - 2).toFixed(1) + ', ' + lastN.getitem(N - 1).toFixed(1));

xtract.xtract_last_n_state_delete(state);

console.log('Reading a descriptor...');

const feature = xtract.xtract_feature_by_name('spectral_centroid');
const descriptor = xtract.xtract_descriptor(feature);
if (feature !== xtract.XTRACT_SPECTRAL_CENTROID || descriptor.name !== 'spectral_centroid') {
    console.log('The spectral centroid is not found by its name');
    process.exit(1);
}
console.log(descriptor.title + ': ' + descriptor.description);

console.log('\nFinished!\n');
