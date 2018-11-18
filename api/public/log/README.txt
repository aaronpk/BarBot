
SELECT *
FROM log
JOIN recipes ON log.recipe_id = recipes.id
JOIN recipe_ingredients ON recipes.id = recipe_ingredients.recipe_id
JOIN ingredients ON ingredients.id = recipe_ingredients.ingredient_id
WHERE ingredients.id = 8
AND log.date_finished > '2017-04-07 20:00:00'
AND log.date_finished < '2017-04-26 14:36:46'

