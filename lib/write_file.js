var fs = require('fs');
function write_file(dir, filename, data, save_dir) {
    db = "module.exports = " + JSON.stringify(data, null, 4);

    var savedDBFile = save_dir + '/' + filename + "." + Date.now() + ".sav";
    var rd = fs.createReadStream(dir + '/' + filename);
    rd.on("error", function (err) {
        console.log("Can not read " + dir + '/' + filename + " : " + err);
        //fs.appendFileSync(filename, ''); 
    });
    var wr = fs.createWriteStream(savedDBFile);
    wr.on("error", function (err) {
        console.log("Can not write " + save_dir + '/' + filename + " : " + err);
    });
    wr.on("close", function (ex) {
        // 2. Writing new file because snapshot is ok ! 
        fs.writeFile(dir + '/' + filename, db, function (err) {
            if (err) {
                console.log(err);
                throw err;
            }
            console.log("saved new version of " + dir + '/' + filename);
        });
    });
    rd.pipe(wr);
}
exports.write_file = write_file;
