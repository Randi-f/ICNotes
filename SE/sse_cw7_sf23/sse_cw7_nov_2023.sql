-- Q1 returns (name,type)
-- Write an SQL query returning the scheme (name,type) ordered by name that lists the name
-- of each country, province, or city in the database, with type being respectively ‘Country’,
-- ‘Province’, ‘City’. A particular name and type pair should only be listed once.
SELECT name, 'Country' AS type
FROM country
UNION
SELECT name, 'Province' AS type
FROM province
UNION 
SELECT name, 'City' AS type
FROM city
ORDER BY name,type
--     CASE
--         WHEN type LIKE 'Country' THEN 1
--         WHEN type LIKE 'Province' THEN 2
--         -- Add more conditions as needed
--         ELSE 99  -- or any other value for rows that don't match the specified conditions
--     END;
;

-- Q2 returns (country,population)
-- Write an SQLs query returning the scheme (country,population) in descending order of popu-
-- lation and then alphabetical order of country, that lists names of island states: i.e. 
-- countries that have no land border with another country.
SELECT country.name, country.population
FROM country
WHERE country.code NOT IN (SELECT borders.country1 
                            FROM borders
                            UNION SELECT borders.country2 FROM borders)
ORDER BY country.population DESC, country.name
;

-- Q3 returns (country,no_neighbours,border_length)
-- 3. Write an SQL query returning the scheme (country,no neighbours,border length) ordered by
-- country, list each name of each country with land borders, together with the number of
-- neighbouring coutries, and the total length of those land borders.
SELECT country.name, COUNT(borders.length) AS no_neighbours, SUM(borders.length) AS border_length
FROM country
    JOIN borders 
    ON country.code IN (borders.country1, borders.country2)
GROUP BY country.name
ORDER BY country.name
;

-- Q4 returns (organization,no_members,population)
-- 4. Write an SQL query returning the scheme (organization,no members,population) ordered by
-- organisation that lists organizations with at least 20 full members, the number of full mem-
-- bers, and the total population of those full members.
-- Missing row: ASEAN Regional Forum, 26, 4411253440
-- 181: Missing row: African Union, 54, 1168241011
-- 182: Missing row: African Union/United Nations Hybrid Operation in Darfur, 47, 3408560319
-- 183: Missing row: African, Caribbean, and Pacific Group of States, 79, 1056301572
SELECT organization.name, COUNT(is_member.organization) AS no_members, SUM(country.population) AS population
FROM organization
JOIN is_member
ON is_member.organization = organization.abbreviation
JOIN country
ON is_member.country = country.code
WHERE is_member.type = 'member'
GROUP BY organization.name
HAVING COUNT(is_member.organization) >= 20
ORDER BY organization.name
;

-- Q5 returns (country,capital,gdp,unemployment,government)
-- 5. Write an SQL query returning the scheme (country,capital,gdp,unemployment,government)
-- ordered by country that lists the name and capital of all countries, together with any infor-
-- mation recorded about the GDP, unemployment rate and type of government.
SELECT country.name, country.capital, economy.gdp, economy.unemployment, politics.government
FROM country
LEFT JOIN economy
ON economy.country = country.code
LEFT JOIN politics
ON politics.country = country.code
ORDER BY country.name
;
-- Afghanistan, Kabul, 20650, 35, Islamic republic
-- 237: Missing row: Akrotiri and Dhekelia, Episkopi Cantonment, , , British sovereign base area
-- 238: Missing row: Albania, Tirana, 12800, 16, parliamentary democracy
-- 239: Missing row: Algeria, Algiers, 215700, 10, republic
-- 240: Missing row: American Samoa, Pago Pago, 462, 29, unincorporated and unorganized territory of the US
-- Missing row: Gaza Strip, Gaza, 1700, ,
-- 188: Missing row: Tokelau, Fakaofo, 2, ,
-- 189: Missing row: West Bank, Ramallah, 3700, ,

-- Q6 returns (country,neighbour,length,rank)
-- 6. Write an SQL query returning the scheme (country,neighbour,length,rank) ordered by coun-
-- try,rank,neighbour that lists the length of all land borders that country named country has
-- with neighbouring country named neighbour. The length of the border should be given,
-- together with the ranking of that border length for country, with the longest border ranked
-- one.
SELECT country.name, neighbour.name AS neighbour, borders.length, RANK() OVER (PARTITION BY country.name ORDER BY borders.length DESC) AS rank
FROM country
JOIN borders
ON country.code IN (borders.country1, borders.country2)
JOIN country AS neighbour
ON neighbour.code IN (borders.country1, borders.country2)
WHERE country.code <> neighbour.code
ORDER BY country.name, rank, neighbour.name
;
-- Afghanistan, Pakistan, 2430, 1
-- 293: Missing row: Afghanistan, Tajikistan, 1206, 2
-- 294: Missing row: Afghanistan, Iran, 936, 3
-- 295: Missing row: Afghanistan, Turkmenistan, 744, 4
-- 296: Missing row: Afghanistan, Uzbekistan, 137, 5


-- Q7 returns (organization,africa,europe)
-- 7. Write an SQL query returning the scheme (organization,africa,europe) ordered by organization,
-- listing the name of all organizations, together with the sum of the land area of full members
-- that falls within Africa and within Europe. The areas must be rounded to the nearest whole
-- number.
SELECT organization.name, COALESCE(ROUND(SUM(country.area*encompasses_africa.percentage/100)),0) AS africa, COALESCE(ROUND(SUM(country.area*encompasses_europe.percentage/100)),0) AS europe
FROM organization
LEFT JOIN is_member
ON organization.abbreviation = is_member.organization
JOIN country
ON is_member.country = country.code
LEFT JOIN encompasses AS encompasses_africa
ON encompasses_africa.country = country.code AND encompasses_africa.continent = 'Africa'
LEFT JOIN encompasses AS encompasses_europe
ON encompasses_europe.country = country.code AND encompasses_europe.continent = 'Europe'
WHERE is_member.type = 'member'
GROUP BY organization.name
ORDER BY organization.name
;
-- Economic and Monetary Community of Central Africa, 5034850, 0
-- 349: Missing row: International Court of Justice, 0, 0
-- 350: Missing row: VisegrÃ¡d Group, 0, 533261
-- 351: Missing row: Group of 7, 0, 1449990
-- 352: Missing row: Organization for Economic Cooperation and Development, 0, 4323924

-- Q8 returns (country,population,year,pc_max,max_year)

;





-- 7. Write an SQL query returning the scheme (organization,africa,europe) ordered by organization,
-- listing the name of all organizations, together with the sum of the land area of full members
-- that falls within Africa and within Europe. The areas must be rounded to the nearest whole
-- number.
-- 8. Write an SQL query returning the scheme (country,population,year,pc max,max year) ordered
-- by country,year that lists historic records of country population in different years. For each
-- records pc max to 1 decimal place the percentage that year’s population is of the maximum
-- historic population of the country, and max year records the year that maximum was reached.