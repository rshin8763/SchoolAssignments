//Ryan Shin
//Database Project
//CS340

var express = require('express');
var mysql = require('./dbcon.js');

var app = express();
var handlebars = require('express-handlebars').create({defaultLayout:'main'});

app.engine('handlebars', handlebars.engine);
app.set('view engine', 'handlebars');
app.set('port', 8762);
app.use(express.static('public'));

app.get('/',function(req,res,next){
  res.render('home');
});

app.get('/persona',function(req,res,next){
  context = {};
  context.header = 'Personas'
  context.tName = 'persona'
  res.render('persona', context);
});

app.get('/skill',function(req,res,next){
  context = {};
  context.header = 'Skills'
  context.tName = 'skill'
  res.render('skill', context);
});

app.get('/character',function(req,res,next){
  context = {};
  context.header = 'Characters'
  context.tName = 'chara'
  res.render('character', context);
});

app.get('/arcana',function(req,res,next){
  context = {};
  context.header = 'Arcana'
  context.tName = 'arcana'
  res.render('arcana', context);
});

app.get('/persona-skill',function(req,res,next){
  context = {};
  context.persona = req.query.persona
  context.header = "'s Skills";
  context.tName = 'has_skill';
  res.render('has-skill', context);
});

app.get('/has_skill',function(req,res,next){
  context = {};
  context.header = "Persona Skill Relationship";
  context.tName = 'has_skill';
  res.render('has_skill', context);
});

app.get('/select', function(req,res,next){
  mysql.pool.query('SELECT * FROM ?', req.query.table, function(err, rows, fields){
    if(err){
      next(err);
      return;
    }
    res.send(JSON.stringify(rows));
  });
});

app.get('/select-arcana-id', function(req,res,next){
  mysql.pool.query('SELECT id, name FROM arcana', function(err, rows, fields){
    if(err){
      next(err);
      return;
    }
    res.send(JSON.stringify(rows));
  });
});

app.get('/select-persona-id', function(req,res,next){
  mysql.pool.query('SELECT id, name FROM persona', function(err, rows, fields){
    if(err){
      next(err);
      return;
    }
    res.send(JSON.stringify(rows));
  });
});

app.get('/select-skill-id', function(req,res,next){
  mysql.pool.query('SELECT id, name FROM skill', function(err, rows, fields){
    if(err){
      next(err);
      return;
    }
    res.send(JSON.stringify(rows));
  });
});

app.get('/select-chara-id', function(req,res,next){
  mysql.pool.query('SELECT id, fname FROM chara', function(err, rows, fields){
    if(err){
      next(err);
      return;
    }
    res.send(JSON.stringify(rows));
  });
});

app.get('/select-persona', function(req,res,next){
        // [req.query.table],
  mysql.pool.query('SELECT p.id, p.name, p.lvl, a.name AS arcana FROM persona p INNER JOIN arcana a ON p.aid = a.id', function(err, rows, fields){
    if(err){
      next(err);
      return;
    }
    res.send(JSON.stringify(rows));
  });
});

app.get('/get-persona', function(req,res,next){
  mysql.pool.query('SELECT p.id, p.name, p.lvl, a.name AS arcana FROM persona p INNER JOIN arcana a ON p.aid = a.id WHERE p.id = ?',[req.query.id], function(err, rows, fields){
    if(err){
      next(err);
      return;
    }
    res.send(JSON.stringify(rows));
  });
});

app.get('/select-skill', function(req,res,next){
        // [req.query.table],
  mysql.pool.query('SELECT * FROM skill', function(err, rows, fields){
    if(err){
      next(err);
      return;
    }
    res.send(JSON.stringify(rows));
  });
});

app.get('/select-arcana', function(req,res,next){
        // [req.query.table],
  mysql.pool.query('SELECT a.id, a.name, c.fname AS confidant, a.lvl FROM arcana a INNER JOIN chara c ON a.cid = c.id', function(err, rows, fields){
    if(err){
      next(err);
      return;
    }
    res.send(JSON.stringify(rows));
  });
});

app.get('/select-chara', function(req,res,next){
  mysql.pool.query('SELECT * FROM chara', function(err, rows, fields){
    if(err){
      next(err);
      return;
    }
    res.send(JSON.stringify(rows));
  });
});

app.get('/select-has_skill', function(req,res,next){
  mysql.pool.query('SELECT p.name AS persona, s.name AS skill, hs.lvl AS level FROM has_skill hs INNER JOIN persona p ON p.id = hs.pid INNER JOIN skill s ON s.id = hs.sid ORDER BY p.name', function(err, rows, fields){
    if(err){
      next(err);
      return;
    }
    res.send(JSON.stringify(rows));
  });
});

app.get('/search-has_skill', function(req,res,next){
  mysql.pool.query('SELECT p.name AS persona, s.name AS skill, hs.lvl AS level FROM has_skill hs INNER JOIN persona p ON p.id = hs.pid INNER JOIN skill s ON s.id = hs.sid WHERE p.id = ? ORDER BY hs.lvl', [req.query.pid], function(err, rows, fields){
    if(err){
      next(err);
      return;
    }
    res.send(JSON.stringify(rows));
  });
});

app.get('/insert-persona', function(req,res,next){
  mysql.pool.query("INSERT INTO persona VALUES (NULL, ?, ?, ?)", 
      [req.query.name, req.query.lvl, req.query.aid], function(err, result){
    if(err){
      next(err);
      return;
    }
    res.send(null);
  });
});

app.get('/insert-skill', function(req,res,next){
  mysql.pool.query("INSERT INTO skill VALUES (NULL, ?, ?, ?)", 
      [req.query.name, req.query.description, req.query.cost], function(err, result){
    if(err){
      next(err);
      return;
    }
    res.send(null);
  });
});

app.get('/insert-arcana', function(req,res,next){
  mysql.pool.query("INSERT INTO arcana VALUES (NULL, ?, ?, ?)", 
      [req.query.name, req.query.cid, req.query.lvl], function(err, result){
    if(err){
      next(err);
      return;
    }
    res.send(null);
  });
});

app.get('/insert-chara', function(req,res,next){
  mysql.pool.query("INSERT INTO chara VALUES (NULL, ?, ?)", 
      [req.query.fname, req.query.lname], function(err, result){
    if(err){
      next(err);
      return;
    }
    res.send(null);
  });
});

app.get('/insert-has_skill', function(req,res,next){
  mysql.pool.query("INSERT INTO has_skill VALUES (?, ?, ?)", 
      [req.query.pid, req.query.sid, req.query.lvl], function(err, result){
    if(err){
      next(err);
      return;
    }
    res.send(null);
  });
});


app.get('/delete', function(req,res,next){
  mysql.pool.query("DELETE FROM ?? WHERE id=?", [req.query.tName, req.query.id], function(err, result){
    if(err){
      next(err);
      return;
    }
    res.send(null);
  });
});


app.get('/edit-page', function(req,res,next){
  var context = {};

    context.tName = req.query.tName
    context.id = req.query.id

    res.render('edit', context);
});

app.get('/update-persona', function(req,res,next){
  var context = {};
  mysql.pool.query("UPDATE persona SET name=?, lvl=?, aid=? WHERE id=?",
    [req.query.name, req.query.lvl, req.query.aid, req.query.id],
    function(err, result){
    if(err){
      next(err);
      return;
    }
    res.send(null);
  });
});

app.use(function(req,res){
  res.status(404);
  res.render('404');
});

app.use(function(err, req, res, next){
  console.error(err.stack);
  res.status(500);
  res.render('500');
});

app.listen(app.get('port'), function(){
  console.log('Express started on http://localhost:' + app.get('port') + '; press Ctrl-C to terminate.');
});
