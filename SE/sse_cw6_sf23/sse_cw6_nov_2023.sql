-- Q1 returns (country,capital,population)
SELECT name, capital, population
FROM country
WHERE population > 100000000
ORDER BY population DESC

;

-- Q2 returns (country,capital,population)
-- Write an SQL query returning the scheme (name,capital,population) in descending order of
-- population that lists all countries which have a name that ends ‘Island’ or ‘Islands’.
SELECT name, capital, population
FROM country
WHERE name LIKE '%Island' OR name LIKE '%Islands'
ORDER BY population DESC
;

-- Q3 returns (continent,country)
-- Write an SQL query returning the scheme (continent,country) ordered by continent,country
-- that lists the name of continents and names of countries which the continent at least partially
-- encompasses.
SELECT encompasses.continent, country.name
FROM encompasses JOIN country
    ON country.code = encompasses.country
WHERE encompasses.percentage > 0
ORDER BY encompasses.continent, country.name
;

-- Q4 returns (continent,country,area)
-- Write an SQL query returning the scheme (continent,country,area) ordered by continent,country
-- that lists the name of continents and names of countries, and the area of land of each country
-- that is within that continent (rounded to the nearest whole number).
SELECT encompasses.continent, country.name, ROUND(country.area * encompasses.percentage / 100) AS roundedArea
FROM encompasses JOIN country
    ON country.code = encompasses.country
ORDER BY encompasses.continent, country.name
;

-- Q5 returns (organization_name,country_name)
-- Write a query that returns the scheme (organization name,country name) ordered by organi-
-- zation name,country name that lists the name of organization and countries which are full
-- members of the organization.
-- SELECT organization.name AS organization_name, country.name AS country_name
-- FROM organization
-- JOIN is_member ON organization.abbreviation = is_member.organization 
-- JOIN country ON organization.country = country.code  
-- WHERE is_member.type = 'member'
-- ORDER BY organization.name, country.name
-- ;
SELECT organization.name AS organization_name, country.name AS country_name
FROM is_member
JOIN organization ON organization.abbreviation = is_member.organization 
JOIN country ON is_member.country = country.code  
WHERE is_member.type = 'member'
ORDER BY organization.name, country.name
;

-- Q6 returns (country,capital,hemisphere)
-- Suppose that we define that a ‘Northern’ hemisphere country as any country that has its
-- capital city located at 0◦ latitude or higher, and all other countries are ‘Southern’ hemisphere.
-- Write an SQL query returning the scheme (country,capital,hemisphere) ordered by country
-- that lists the names of countries and their capital cities, and the hemisphere to which they
-- belong.
SELECT DISTINCT country.name, country.capital, 
    CASE WHEN city.latitude > 0 THEN 'Northern' ELSE 'Southern' END AS hemisphere
FROM country
JOIN city ON country.capital = city.name AND country.code = city.country
ORDER BY country.name,country.capital
;

-- Q7 returns (country,neighbour,length)
-- Write an SQL query that returns the scheme (country,neighbour,length)) ordered by country,
-- and then in descending order of length, listing the name of coutries with their neighbouring
-- country name, and the length of the land border that they share.


-- SELECT country.name, neighbour.name AS neighbour, borders.length
-- FROM borders
-- JOIN country ON borders.country1 = country.code
-- JOIN country AS neighbour ON borders.country2 = neighbour.code
-- ORDER BY country.name, length DESC
-- ;
SELECT country.name, neighbour.name AS neighbour, borders.length
FROM country
JOIN
    borders ON borders.country1 = country.code OR borders.country2 = country.code
JOIN
    country AS neighbour ON
    CASE
        WHEN borders.country1 = country.code THEN borders.country2
        ELSE borders.country1
    END = neighbour.code
ORDER BY country.name, length DESC
;

-- Q8 returns (country,capital,feature,distance)
-- d = arccos(sin(lat1 ) sin(lat2 ) + cos(lat1 ) cos(lat2 ) cos(long2 − long1 ))r
SELECT country.name AS country, city.name AS capital, feature.name AS feature, FLOOR(CAST(ACOS(SIN(RADIANS(city.latitude)) * SIN(RADIANS(feature.latitude)) + COS(RADIANS(city.latitude)) * COS(RADIANS(feature.latitude)) * COS(RADIANS(feature.longitude) - RADIANS(city.longitude))) * 3959 AS numeric)) AS distance
FROM city, feature, country
WHERE city.name = country.capital 
-- AND city.name = 'Andorra la Vella' AND feature.name = 'Valira'
-- Andorra, Andorra la Vella, Valira, 0
-- Akrotiri and Dhekelia, Episkopi Cantonment, Olympos, 14
-- AND city.name = 'Episkopi Cantonment' AND feature.name = 'Olympos'
    AND ACOS(SIN(RADIANS(city.latitude)) * SIN(RADIANS(feature.latitude)) + COS(RADIANS(city.latitude)) * COS(RADIANS(feature.latitude)) * COS(RADIANS(feature.longitude) - RADIANS(city.longitude))) * 3959 <= 25
ORDER BY country.name, city.name, feature.name
;
