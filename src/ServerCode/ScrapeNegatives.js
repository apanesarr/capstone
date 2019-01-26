var Scraper  = require('images-scraper'),
    google   = new Scraper.Google();

var fs      = require('fs'),
    request = require('request');

var download = function (uri, filename, callback) {
    request.head(uri, function (err, res, body) {
        request(uri).pipe(fs.createWriteStream(filename)).on('close', callback);
    });
};

if (process.argv.length != 4) {
    console.log('Wrong # of arguments');
    console.log('Usage node ScrapeNegatives.js [KEYWORD] [NUMBER OF IMAGES]');
}

let keyword = process.argv[2];
let num     = process.argv[3];

google.list({
    keyword: keyword,
    num: num,
    detail: true,
    nightmare: {
        show: true
    }
})
.then(function (res) {
    for (let i = 0; i < res.length; i++) {
        let name = keyword + i + '.jpg';
        download(res[i].url, name, () => {});
    }
}).catch(function (err) {
    console.log('err', err);
});