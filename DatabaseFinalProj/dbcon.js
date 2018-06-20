var mysql = require('mysql');
var pool = mysql.createPool({
  connectionLimit : 10,
  host            : 'classmysql.engr.oregonstate.edu',
  user            : 'cs340_shinr',
  password        : '5534',
  database        : 'cs340_shinr',
});

module.exports.pool = pool;

    
